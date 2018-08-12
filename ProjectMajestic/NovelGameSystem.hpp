#pragma once

#include "Main.hpp"
#include "IO.hpp"

class NovelGameParagraph {
public:
	friend class NovelGameSystem;

	const float dialogueHeight = 200.0f;
	const Time stringCharAppearTime = milliseconds(20);

	void loadFromScriptFile(string filename) {
		valid = false;
		ifstream fin(filename);
		string str;
		if (fin.bad()) {
			mlog << Log::Error << "[NovelGameParagraph] File \"" << filename << "\" open failed" << dlog;
			return;
		}
		// UTF-8 Signature (EF BB BF)
		if (fin.get() == 0xEF)
			fin.ignore(2);
		else
			fin.unget();
		while (!getline(fin, str).eof()) {
			if (str == "")
				continue;
			size_t pos = str.find('#');
			if (pos == string::npos)
				commands.push_back(make_pair(str, ""));
			else
				commands.push_back(make_pair(str.substr(0, pos), str.substr(pos + 1)));
		}
		valid = true;
	}

	void start() {
		if (!valid)
			return;
		running = true;

		nextCommand = 0;
		wantUpdateClick = false;
		runUntilPause();
	}

	void click() {
		wantUpdateClick = true;
	}

	void runUntilPause() {
		while (running) {
			if (nextCommand >= commands.size()) {
				running = false;
				break;
			}

			auto comm = toupper(commands[nextCommand].first);
			auto& param = commands[nextCommand].second;
			if (comm == "TEXT") {
				text = u8ToSfString(StringParser::replaceSubString(param, { { "\\n", "\n" } }));
				stringCharLastAppearTime = milliseconds(0);
				stringTruncatedCnt = 0;
			}
			else if (comm == "TITLE")
				title = u8ToSfString(param);
			else if (comm == "SPEAKER") {
				if (param == "")
					speaker = nullptr;
				else {
					if (speaker == nullptr)
						speaker = make_shared<Texture>();
					speaker->loadFromFile("assets/textures/novelgame/" + param);
				}
			}
			else if (comm == "PAUSE") {
				nextCommand++;
				break;
			}

			nextCommand++;
		}
	}

	void updateLogic() {
		if (!running)
			return;

		if (wantUpdateClick) {
			wantUpdateClick = false;
			runUntilPause();
			stringCharLastAppearTime = milliseconds(0);
			stringTruncatedCnt = 0;
		}

		stringCharLastAppearTime += logicIO.deltaTime;
		if (stringTruncatedCnt < text.getSize() &&
			stringCharLastAppearTime >= stringCharAppearTime) {
			stringCharLastAppearTime = milliseconds(0);
			stringTruncatedCnt++;
		}
	}

	void runImGui() {
		if (!running)
			return;
		if (font != nullptr)
			imgui::PushFont(font);
		imgui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
		imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
		imgui::PushStyleColor(ImGuiCol_WindowBg, Color(0, 0, 0, 128));

		imgui::SetNextWindowPos(ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y - 10.0f),
								ImGuiCond_Always, ImVec2(.5f, 1.0f));
		imgui::SetNextWindowSize(ImVec2(imgui::GetIO().DisplaySize.x - 20.0f, dialogueHeight), ImGuiCond_Always);
		imgui::Begin("##NovelgameMain", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

		imgui::TextUnformatted(reinterpret_cast<const char*>(text.substring(0, stringTruncatedCnt).toUtf8().c_str()));

		if (imgui::IsWindowHovered() && logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed) {
			click();
		}

		imgui::End();

		if (title != "") {
			imgui::PopStyleVar();
			imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
			imgui::SetNextWindowPos(ImVec2(10.0f, imgui::GetIO().DisplaySize.y - 10.0f - dialogueHeight - 5.0f),
									ImGuiCond_Always, ImVec2(.0f, 1.0f));
			imgui::Begin("##NovelgameTitle", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
			imgui::TextUnformatted(reinterpret_cast<const char*>(title.toUtf8().c_str()));
			imgui::End();
		}

		imgui::PopStyleVar(2);
		imgui::PopStyleColor();

		if (speaker != nullptr) {
			ImVec2 size = imgui::GetIO().DisplaySize;
			imgui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);
			imgui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
			imgui::SetNextWindowBgAlpha(.0f);
			imgui::SetNextWindowPos(size, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
			imgui::SetNextWindowSize(speaker->getSize());
			imgui::Begin("##SpeakerOverlay", nullptr,
						 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize);
			imgui::GetWindowDrawList()->AddImage((ImTextureID)speaker->getNativeHandle(),
												  ImVec2(size.x - speaker->getSize().x, size.y - speaker->getSize().y),
												  size);
			imgui::End();
			imgui::PopStyleVar(2);
		}
		if (font != nullptr)
			imgui::PopFont();
	}

private:
	string strid;

	bool running, valid = false, paused;
	bool wantUpdateClick;
	ImFont* font;

	Time stringCharLastAppearTime;

	int stringTruncatedCnt;
	String text, title;
	shared_ptr<Texture> speaker;

	int nextCommand;
	vector<pair<string, string>> commands;

};


class NovelGameSystem {
public:

	void preInitalaize() {

		

		font = imgui::GetIO().Fonts->AddFontFromFileTTF(
			"C:/Windows/Fonts/msyh.ttc",
			24.0f,
			nullptr,
			imgui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon()
		);

	}

	void loadScriptFromFile(string filename, string id) {
		mlog << "[NovelGameSystem] Loading paragraph \"" << id << "\" from file \"" << filename << "\"" << dlog;
		if (paragraphs.find(id) != paragraphs.end())
			mlog << Log::Warning << "[NovelGameSystem] Warning: Paragraph with id \"" << id << "\" already exists, skipping" << dlog;
		auto i = paragraphs.insert(make_pair(id, NovelGameParagraph())).first;
		auto& para = i->second;
		para.loadFromScriptFile(filename);
		if (!para.valid) {
			mlog << Log::Error << "[NovelGameSystem] Paragraph returned error upon loading" << dlog;
			paragraphs.erase(i);
		}
		para.strid = id;
		para.font = font;
	}

	void start(string id) {
		auto i = paragraphs.find(id);
		if (i == paragraphs.end()) {
			mlog << Log::Error << "[NovelGameSystem] When starting: Paragraph with id \"" << id << "\" not found" << dlog;
			return;
		}
		i->second.start();
		current = &(i->second);
	}

	void updateLogic() {
		if (current != nullptr) {
			current->updateLogic();
			if (!current->running)
				current = nullptr;
		}
	}

	void runImGui() {
		if (current != nullptr)
			current->runImGui();
	}

	// Empty string if none
	string getCurrentParagraphId() {
		if (current == nullptr)
			return "";
		else
			return current->strid;
	}


private:

	ImFont * font;
	map<string, NovelGameParagraph> paragraphs;
	NovelGameParagraph* current;

};


NovelGameSystem novelGameSystem;
