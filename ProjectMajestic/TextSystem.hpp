#pragma once

#include <fstream>
#include <string>
#include <map>
#include <SFML/System.hpp>

#include "LogSystem.hpp"

using namespace std;
using namespace sf;

class TextSystem {
public:

	bool loadFromFile(const string& filename) {
		mlog << "[TextSystem] Loading langfile:" << filename << dlog;
		ifstream fin(filename);
		if (!fin.good()) {
			return false;
			mlog << Log::Error << "             File Open Failed!" << dlog;
		}

		// UTF-8 Signature
		if (fin.get() == 0xEF)
			fin.ignore(2);
		else
			fin.unget();

		string str;
		langs.clear();
		while (!getline(fin, str).eof()) {
			size_t pos;
			if (str.size() == 0 || str[0] == '#' || (pos = str.find('=')) == string::npos)
				continue;
			string id = str.substr(0, pos), cont = str.substr(pos + 1);
			mlog << "             Loaded object: " << id << " = " << cont << dlog;
			langs.insert(make_pair(id, cont));
		}

		return true;
	}

	const char* get(string id) {
		auto i = langs.find(id);
		if (i == langs.end())
			return empty.c_str();
		else
			return i->second.c_str();
	}

	String getSfString(string id) {
		auto i = langs.find(id);
		if (i == langs.end())
			return String();
		else
			return String::fromUtf8(i->second.begin(), i->second.end());
	}

private:
	// Id - TextUtf8
	map<string, string> langs;
	string empty;

};

TextSystem text;
