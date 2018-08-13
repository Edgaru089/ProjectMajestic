#pragma once

#include "NovelGameSystem.hpp"
#include "IO.hpp"
#include "AssetManager.hpp"


////////////////////////////////////////
void NovelGameParagraph::loadFromScriptFile(const string& filename) {
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
		if (fin.bad()) {
			mlog << Log::Error << "[NovelGameParagraph] File \"" << filename << "\": Bad IO stream" << dlog;
			return;
		}
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


////////////////////////////////////////
void NovelGameParagraph::start() {
	if (!valid)
		return;
	running = true;

	nextCommand = 0;
	wantUpdateClick = false;
	runUntilPause();
}


////////////////////////////////////////
void NovelGameParagraph::click() { wantUpdateClick = true; }


////////////////////////////////////////
void NovelGameParagraph::runUntilPause() {
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
				speaker->loadFromFile(assetManager.getAssetFilename(param));
			}
		}
		else if (comm == "SPEAKERFILE") {
			if (param == "")
				speaker = nullptr;
			else {
				if (speaker == nullptr)
					speaker = make_shared<Texture>();
				speaker->loadFromFile(param);
			}
		}
		else if (comm == "PAUSE") {
			nextCommand++;
			break;
		}

		nextCommand++;
	}
}


////////////////////////////////////////
void NovelGameParagraph::updateLogic() {
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


////////////////////////////////////////
void NovelGameParagraph::runImGui() {
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


////////////////////////////////////////
void NovelGameSystem::preInitalaize() {
	mlog << "[NovelGameSystem] Loading Novelgame Font..." << dlog;

//	ImFontAtlas::GlyphRangesBuilder builder;
//	builder.AddRanges(imgui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
//	builder.AddText(
//u8R"(ذ��ؤ����ز�ؼ��ߵ˰���͹¬��ߴ����ʸէ������������������������ߺ��͢������������������ױ��Ѵ���ȶ��Ͼ�������ѱ�Ҿ����Ϳٶ󹯰��տ���������
//ܽ��έ��о����ɼ��辸�ϻ��±Ф֨��Ż����Ǻ�ǿ��ض�˱�ĵ�ӵ��ŴѸ���鱷�ͮ���ٱ���®���������̭�����ڲ׻�����թ��ƨ׹��梶�γõ�Կ�����ƺ����š��
//׾Ĵ��������ɻ������̦����ö�㺼��������Ź��׿꼰������������ؿ����˱����ձ��������³ޱ���ɲ���ٷ��Ͱ�������ű�����å�������Ţ�������ӳ����������
//�뻡��ªİ��ķʭ����԰�����������ɺ�ÿ���Ю���������ײ������������ӫ��ջ��դ���ϲ���ɰ����Ÿ���Ű������������ѫ����Ӵ�ĸƶ����վ�ťձ�����ζ�����
//��������̥����������쪹�����¦˸�����������ѻ������½���������ʺѷ��Ҧ���麧�Ű����乡�溴Ԭ�ƴ�ֿ��ͱ������ݩç����Өݺ������˨Φ׮����������ѳ����
//�벸�ް������������������������¸�߼�í�����ʰ��޾�����Ҩ�򱪰������ŧ������ԧ�������Թ�ի���ɴɸ������л��ӽ���ɬ������ŵ��̻׻��ˡ�俥������
//�Ŵ��ഷ�Ⲻ������Ҵ����������Ȣ���ή��өǬ��������������������˶�ݿ���­��������������ֺ����������������ơɶХ���ߴޱ�Ӥ��������աϳ���ýջ�
//�����˿����������㲰�϶����Ḭ�ݻ˲���������Ȭ�ֲ��캸���������ʻ���Ԩ�����ٵ��̺��������ϵ�г�ɸ�����ν��ξ�������Ĵ±�����׺�����������߿���Ҿ��
//�����Ҽɦ��ļ���ٺ���Ҭ��׵�������ڼ��������������������������������˻��Ѹ��������Ƕ������п�����������������Ҹ����⬱��ض�����������챺��
//�河��������㵻�Ԣ�Ѿ��Ͱ�Ϭ��ý�������µ���ɧɪ�Ĺ���Ƹ��ѥ�б�������Ш���������ҵ�������Ƶ����������Ͼ���ζ�������Ӽ�����������ê׶�Ƕ�������
//���������������Գӱɷ�������������վ���������������į�����޹�����ϱ�����ͽ�׸������ġ���ᰪ��ε����Ž͵����̼ԯϽ������ӻ����᣶���Ѭ��������ɮ��
//�����ô�������������̷�������ӧ���ô����ܶ�׫�����̷����Ϸ��㴼�İ����˻������Ы������ٴ������¨�����������̱����̶�ʰ��˳�����㾰�������Ǵ�׺�
//����ԥ�Ժ�������Ѧޱ�溲ج����ư�����޼�������������ǭ�´���������ž��ȳ���Ǳ���и����������������̴���ײt˲ͫ����̣�󰺿����κ���ܻվ�������
//�Ӱ�ų����ź��հ������ٽ�赴�����Ģ���������з��Ѣ��������Ρ��ŴƩ������պ��ȿ��)");
//	ImVector<ImWchar> range;
//	range.clear();
//	builder.BuildRanges(&range);
//	font = imgui::GetIO().Fonts->AddFontFromFileTTF(
//		assetManager.getAssetFilename("novelgame_font").c_str(),
//		28.0f,
//		nullptr,
//		range.Data
//	);

	font = imgui::GetIO().Fonts->AddFontFromFileTTF(
		assetManager.getAssetFilename("novelgame_font").c_str(),
		28.0f,
		nullptr,
		imgui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon()
	);

	mlog << "[NovelGameSystem] Loading Novelgame Scripts..." << dlog;
	for (auto& i : assetManager.getAssetMapper()) {
		if (i.second.type == "NOVELGAME_SCRIPT") {
			const string& id = i.second.strid, filename = i.second.filename;
			size_t pos = id.find('_');
			if (pos != string::npos)
				loadScriptFromFile(filename, id.substr(pos + 1));
			else
				loadScriptFromFile(filename, id);
		}
	}
}


////////////////////////////////////////
void NovelGameSystem::loadScriptFromFile(const string& filename, const string& id) {
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
	mlog << "[NovelGameSystem] Loaded " << para.commands.size() << " commands" << dlog;
}


////////////////////////////////////////
void NovelGameSystem::start(const string& id) {
	auto i = paragraphs.find(id);
	if (i == paragraphs.end()) {
		mlog << Log::Error << "[NovelGameSystem] When starting: Paragraph with id \"" << id << "\" not found" << dlog;
		return;
	}
	i->second.start();
	current = &(i->second);
}


////////////////////////////////////////
void NovelGameSystem::updateLogic() {
	if (current != nullptr) {
		current->updateLogic();
		if (!current->running)
			current = nullptr;
	}
}


////////////////////////////////////////
void NovelGameSystem::runImGui() {
	if (current != nullptr)
		current->runImGui();
}


////////////////////////////////////////
string NovelGameSystem::getCurrentParagraphId() {
	if (current == nullptr)
		return "";
	else
		return current->strid;
}

