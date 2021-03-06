/********************************************************************************
 *    Copyright (C) 2017 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include <fairmq/tools/Process.h>

#include <boost/process.hpp>

#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

namespace fair
{
namespace mq
{
namespace tools
{

/**
 * Execute given command in forked process and capture stdout output
 * and exit code.
 *
 * @param[in] cmd Command to execute
 * @param[in] log_prefix How to prefix each captured output line with
 * @return Captured stdout output and exit code
 */
execute_result execute(const string& cmd, const string& prefix, const string& input)
{
    execute_result result;
    stringstream out;

    // print full line thread-safe
    stringstream printCmd;
    printCmd << prefix << " " << cmd << "\n";
    cout << printCmd.str() << flush;

    out << prefix << cmd << endl;

    // Execute command and capture stdout, add prefix line by line
    boost::process::ipstream c_stdout;
    boost::process::opstream c_stdin;
    boost::process::child c(
        cmd, boost::process::std_out > c_stdout, boost::process::std_in < c_stdin);

    // Optionally, write to stdin of the child
    if (input != "") {
        this_thread::sleep_for(chrono::milliseconds(100));
        c_stdin << input;
        c_stdin.flush();
    }

    string line;
    while (getline(c_stdout, line))
    {
        // print full line thread-safe
        stringstream printLine;
        printLine << prefix << line << "\n";
        cout << printLine.str() << flush;

        out << prefix << line << "\n";
    }

    c.wait();

    // Capture exit code
    result.exit_code = c.exit_code();
    out << prefix << " Exit code: " << result.exit_code << endl;

    result.console_out = out.str();

    // Return result
    return result;
}

} /* namespace tools */
} /* namespace mq */
} /* namespace fair */
