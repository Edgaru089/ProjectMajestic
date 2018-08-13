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
//u8R"(Ø°µóØ¤´õ¸êØ²ÂØ¼¥ÈßµË°¬º»Í¹Â¬°Èß´Ãó°¼ÇôÊ¸Õ§¶û·ëĞş°îÓØĞÏÓóÉÖÀôÒÄÓõÂÀßºÒÙÍ¢Æù¾ÊÖÙÂ×ÒÁÀßĞñĞÙÙì×±º¥Ñ´»äÑÈ¶ïËÏ¾÷³ÚÚåÍÔÑ±ÈÒ¾ÁÂêÈÍ¿Ù¶ó¹¯°âÂÕ¿²ÎëÒÖÄâÊã
//Ü½ÎßÎ­½æĞ¾°ÅÕÈÉ¼Î×è¾¸¦Ï»ĞùÂ±Ğ¤Ö¨·ÍÅ»ÄÅÒ÷ÇºÎÇ¿ÔÒØ¶ÚË±á«ÄµÓÓµèËÅ´Ñ¸ØÖâµé±·ğ¯Í®¾ÄÅÙ±ÓÁßÂ®Èò¶Ò×ÆãåÅæÌ­Á¤ÂÙĞÚ²×»¦³À×çÕ©º±Æ¨×¹¼Ëæ¢¶ÊÎ³ÃµØÔ¿ÀÅ÷ÍØÆºÀ¤ÖôÅ¡·÷
//×¾Ä´ŞÖÜÔÎô¿ÁÉ»¹¶°ú×ÂÌ¦Í÷ÊàÃ¶·ãº¼Óô·¯ÄÎÑÙÅ¹Æç×¿ê¼°¥¹¾ºÇÁüÉëÖäÅØ¿§ÅÁÕË±áÖü·Õ±üÔÀÏÀ½ÄÂÂ³Ş±°¹ôÉ²ëÈÃÙ·ŞÎÍ°¹·¾ÄüÅÓÅ±¸í¾Î×äÃ¥¾æ¹Á¾ÚÆüÅ¢ÃÚÕÓÕúÇÓ³èÍğñÃÆí¹îÖã
//Ìë»¡ÃÖÂªÄ°º¯Ä·Ê­ÈşÉğ¾Ô°íÒïÆõ·¡çèÁáÉºÊÃ¿½¹°Ğ®¹¸¶âÕü¾£È×²ç¼ÔÒğÜîÜñÜù»çÓ«ÀóÕ»¸ÌÕ¤Äû¼Ï²ª¼íÉ°±ÃÑâÅ¸Öá¾ÂÅ°ÃÁíïßÖêÇÕÑÖÑÑ«¶ßßäÓ´ÓÄ¸Æ¶ÛÄÆÇÕ¾ûÅ¥Õ±ÇâïõÇÎ¶íÀşºî
//»²ÑÜÅßëÊÌ¥Õø¶üÂÍŞÈ×Éìª¹ëÃö×ÑÂ¦Ë¸ìÅÍİÆâÏÑÂåÊÑ»ĞÌñĞô»Â½ëÎÜìô»åÆÁÊºÑ·ÔÉÒ¦ÄÈÔéº§ÔÅ°ÒÇØÄä¹¡Îæº´Ô¬°Æ´ìÖ¿µ·Í±°£¹¢Äôİ©Ã§À³ÀòÓ¨İº°ğÆÜèëË¨Î¦×®¼Ö×ÃÔÒÅéÀùÑ³³ÑÏø
//ßë²¸ÌŞ°öÑÁÅÏò¼ò½ò¾Á¨ÆÔÑìÑäºßËôÇÍßó¾şÂ¸Ôß¼ØÃ­°±ïø°Ê°³ÁŞ¾óÒóËÊÒ¨²ò±ª°ä¿èÒÈÆêÅ§¹äÇäÍÒÔ§ÄÙÁèÆàÖÔ¹ùÕ«ÕîÎÉ´É¸áÀÓÆÖÎĞ»ÁµÓ½§ÌéÉ¬º·ÃõÇÏÅµ·ÌÌ»×»ËîË¡Ãä¿¥ËöôïÁğ
//ÀÅ´ëŞà´·Éâ²ºÄíÆşµàÒ´ÖÀµ§²ô¿±ÁÄÈ¢Áâ·ÆÎ®ÆĞÓ©Ç¬ÏôÈø¹½±ò¹£ÎàËó²ÜÔÍĞïÏá¹èË¶Éİ¿øØÒÂ­±ë¿ôÎîÂü»ŞÃá·ÈÆèÖº¿ĞÇùòÇòÈÖû»£†ªÍÙÆ¡É¶Ğ¥ÆéÂß´Ş±ÀÓ¤ÉŞîíîõÂÁÕ¡Ï³Ãú½Ã½Õ»à
//óÏóÔÙË¿şÇû¶µĞÆÅÇáã²°ÏÏ¶æÁ²ôá¸¬Òİ»Ë²ş¼ÀÅëÊüâÖÈ¬ÑÖ²û¾ìº¸»ÀºèÑÄÊçÌÊ»´ÏıÔ¨Òù´¾ÓÙµíäÌº­µëã²Íï¼ÅÖÏµıĞ³ñÉ¸¤µ»ÚËÎ½ÑèÎ¾¶éÓçÍñÆÄ´Â±Á×ÛÕÀ×º³²ÁÕ×ÁÇí×áÑß¿«À¿Ò¾Åí
//´§²ó´êÒ¼É¦ºùÄ¼½¯µÙº«ÀâÒ¬·Ù×µ¹×ÀÆÍÖËÚ¼¬º¨ËÖÏõÁò¼Õö¨ÇÌÔäÌÄÎú¶¦Ôû¶ôÁÀ³ë°ÏõË»×ÑÑ¸ò¾éÓ÷ÌäĞúÇ¶¸³Êê´Íï±Ğ¿ÉûêşµªÂÈÊò·¤ë¹ÔÁÓâëçÒ¸ÍóĞÉâ¬±¹¶Ø¶»Á¡»¾¿¢ÏèµìËì±ºÖÍ
//Ïæ²³ÃìÀ£½¦ÅÈãµ»ÌÔ¢½Ñ¾½¹Í°ùÏ¬°¯Ã½ÃÄĞöÃåÀÂµŞÂÆÉ§ÉªğÄ¹åÌÂÆ¸ÕåÑ¥°Ğ±ÍİïÆÑÈØĞ¨´»¿¬é­Àãé¹ÀÒµâÅğµï·ø¼­Æµ¶ÃÄÀÃéÊÈàÂÏ¾»ûõÎ¶åòÚÎÏÍÉÓ¼ĞáÎËàÍÊğÊñ»ÏÃª×¶ÏÇ¶§ÃÌÖÉÍÇ
//¿ê¿ıÑÃÄåÈùÏÙÅôÒŞÔ³Ó±É·³ûâÉÁóÙ÷±ÔÀª³Õ¾¸ÌÜÀìÒçËİÈÜ×ÒÄçÄ¯¿ú¿ßÇŞ¹ÓÂãÃıÏ±¼µ¸¿çÍ½Ë×¸°¾ºÕÄèÄ¡ÂûÕá°ªÎõÎµ¾¤é»éÅ½Íµú²ê¼îÌ¼Ô¯Ï½´ÆÊûàÒÓ»²õàÖá£¶ÆÌòÑ¬¹¿»şóïÓßÉ®·õ
//´ñÎÁÕÃ´âÊşäöÑú¿¶¹ÑÁÈÌ·ºÖÍÊËíµÕÓ§ÄìÁÃ´éÇËÇÜ¶Õ×«°°ÈïÔÌ·®ÕÁéÏ·óÍã´¼¿Ä°õÄë±ïË»³°àÚòğĞ«òò»ÈòùºÙ´±Ä÷¸ä»üÂ¨±ìÀğöê°ı±ñÁöÌ±ÁİÅìÌ¶ÁÊ°ÄÅË³ºÀ½³Îã¾°ÃÔ÷ôæÈìÇ´º×º©
//ÂÄæÒÔ¥çÔº³ÀŞÉÃÀÙÑ¦Ş±Çæº²Ø¬³÷³ÈÆ°ó¨»ôö®ÕŞ¼½õâõåó¡ó¦ÃøÔëğĞÇ­ÄÂ´ÛÅñ¸İÀéÈåÉÅ¾¨ñ«È³²ÚÁÇ±ôº¶Ğ¸ÁşçÖıˆº¾ÃêÃÊéÜéİÌ´½¸Á×²tË²Í«µÉÊïÌ£ó¬ó°º¿ÉÄÁÍÎº´ØÀÜ»Õ¾ôëüëıöù
//ÃÓ°©Å³»íÍÎÅºÌÙÕ°Ïù÷¢ñ®ÆÙ½óèµ´ÁÔÜÄõÄ¢Ôå±î²äµÅô¤²¾Ğ·ÃÒÑ¢¸ş÷ŞÈÁÈäÎ¡ÁÛÅ´Æ©ÅùõïËèÕºÏâÈ¿´£)");
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

