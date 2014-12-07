/*
 *  Copyright (C) 2014 Philippe Groarke.
 *  Author: Philippe Groarke <philippe.groarke@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#ifndef IRC_H_
#define IRC_H_

#include <atomic>
#include <string>

using namespace std;

class Irc {
public:
        Irc(const string& nick, const string& usr, const string& address,
                const string& channel, const string& pass = "",
                const string& port = "6667");
        Irc(const Irc& obj);
        virtual ~Irc();

        void start();
        void say(const string& msg);

        atomic<int> socket_; //socket descriptor
        string nick_;
        string usr_;
        string address_;
        string channel_;
        string pass_;
        string port_;

private:
        bool ircConnect();
        void sendConnect();
        bool isConnected(const string& msg);
        bool sendData(const string& msg);
        string recvData();
        void sendPong(const string& msg);
        void outputMsg(const string& msg);
};

#endif /* Irc_H_ */
