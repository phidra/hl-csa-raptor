#include <iostream>

#include "timetable.hh"
#include "raptor.hh"
#include "connection_scan.hh"
#include "logging.hh"
#include "file_util.hh"

void usage_exit (char **argv) {
    auto paragraph = [](std::string s, int width=80) -> std::string {
        std::string acc;
        while (s.size() > 0) {
            int pos = s.size();
            if (pos > width) pos = s.rfind(' ', width);
            std::string line = s.substr(0, pos);
            acc += line + "\n";
            s = s.substr(pos);
        }
        return acc;
    };
    
    std::cerr <<"Usage: "<< argv[0]
              <<" [options] gtfs_dir\n"
              <<"\n";
    exit(1);
}

std::vector<std::string> get_args(int argc, char **argv) {
    std::vector<std::string> a;
    for (int i = 0; i < argc; ++i) {
        std::string s(argv[i]);
        if (s[0] == '-') continue; // option
        a.push_back(s);
    }
    return a;
}

bool has_opt(int argc, char **argv, std::string opt) {
    assert(opt[0] == '-');
    for (int i = 0; i < argc; ++i) {
        std::string s(argv[i]);
        if (s == opt) return true;
    }
    return false;
}

std::string get_opt(int argc, char **argv,
                    std::string opt_prefix, std::string dft) {
    int len = opt_prefix.size();
    assert(opt_prefix[0] == '-'
           && (opt_prefix[len-1] == '=' || opt_prefix[len-1] == ':'));
    for (int i = 0; i < argc; ++i) {
        std::string s(argv[i]);
        if (s.size() >= len && s.substr(0, len) == opt_prefix) {
            return s.substr(len);
        }
    }
    return dft;
}

int get_int_opt(int argc, char **argv,
                std::string opt_prefix, int dft) {
    return std::stoi(get_opt(argc, argv, opt_prefix, std::to_string(dft)));
}

typedef pareto_rev<int> pset;

int main (int argc, char **argv) {
    logging main_log("--");

    // ------------------------ usage -------------------------
    std::vector<std::string> args = get_args(argc, argv);
    if (args.size() != 2) {
        usage_exit(argv);
    }
    std::string dir{args[1]};
    std::cerr <<"--------------- "<< dir <<" ---------------------\n";
    dir += "/";

    // ------------------------ time -------------------------
    main_log.cerr() << "start\n";
    double t = main_log.lap();

    //std::cerr <<"first rands: "<< rand() <<" "<< rand() <<" "<< rand() <<"\n";


    // ------------------------- load timetable ----------------------
    /*
    timetable ttbl{args[1], args[2],
            dir+"calendar.txt", dir+"calendar_dates.txt",
            dir+"trips.txt", dir+"stop_times.txt", dir+"transfers.txt", true};
    */
    timetable ttbl{dir+"stop_times.csv.gz",
            dir+"walk_and_transfer_inhubs.gr.gz",
            dir+"walk_and_transfer_outhubs.gr.gz",
            dir+"transfers.csv.gz", true};
    //dir+"walking_and_transfers.gr", t_from, t_to};
    std::cerr << ttbl.n_r <<" routes, "<< ttbl.n_st <<" sations, "
              << ttbl.n_s <<" stops\n";
    main_log.cerr(t) << "timetable\n";
    t = main_log.lap();
    //exit(0);

    // --------------- earliest arrival time through Raptor and CSA ---------
    raptor rpt(ttbl);
    main_log.cerr(t) << "raptor initialized\n";
    t = main_log.lap();

    timetable rev_ttbl(ttbl);
    rev_ttbl.check();
    rev_ttbl.reverse_time();
    raptor rev_rpt(rev_ttbl);
    main_log.cerr(t) << "rev raptor initialized\n";
    t = main_log.lap();

    const bool hub=true, trf=false;
    const int chg=std::stoi(get_opt(argc, argv, "-min-change-time=", "60")),
        km=48;

    uint64_t sum = 0, n_ok = 0;


    //* Print a journey
    if (has_opt(argc, argv, "-journey")) {
        int src = std::stoi(get_opt(argc, argv, "-src=", "-1"));
        if (src == -1)
            src = ttbl.id_to_station[get_opt(argc, argv, "-src-id=", "")];
        int dst = std::stoi(get_opt(argc, argv, "-dst=", "-1"));
        if (dst == -1)
            dst = ttbl.id_to_station[get_opt(argc, argv, "-dst-id=", "")];
        int t = std::stoi(get_opt(argc, argv, "-t=", "0"));
        bool hubs = has_opt(argc, argv, "-hubs");

        int arr = rpt.earliest_arrival_time(src, dst, t, hubs, ! hubs, chg);
        std::cout <<" -------- "<< (hubs ? "HL_" : "") <<"Raptor "
                  <<"from "<< src << "=" << ttbl.station_id[src] <<" at "<< t;
        rpt.print_journey(dst, std::cout, chg);
    }

    // ------------------------ end -------------------------
    main_log.cerr() << "end "<< dir <<"\n";

}
