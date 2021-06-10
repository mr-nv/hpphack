#include "framework.h"

const auto IMAGE_DEFAULT_COLOR = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);

// Send announcements of functions included in rename_me.cpp:
extern void DownloadThread();
extern void LookingProcessThread();
extern void InjectingThread();

// Send announcements of functions included in this code module:
std::vector<std::string> GetCheatNamesList();
bool vector_getter(void* vec, int idx, const char** out_text);
void SetWhiteTheme();
void SetDarkTheme();
void SetFrameRounding(int value);

CDrawing::CDrawing()
{
	memset(&m_draw_settings, 0, sizeof(m_draw_settings));

	const auto path = GetLoaderSettingsPath();

	m_draw_settings.themenum = GetValueInt("Theme", "ThemeNum", "0", path.c_str());
	m_draw_settings.rounding = GetValueInt("Theme", "FrameRounding", "6", path.c_str());
	g_GlobalVars.injectData.nocache = GetValueBool("Inject", "NoCache", "1", path.c_str());
	g_GlobalVars.injectData.close = GetValueBool("Inject", "CloseAfterInject", "0", path.c_str());

	switch (m_draw_settings.themenum)
	{
	case WhiteTheme:
		SetWhiteTheme();
		break;
	case DarkTheme:
		SetDarkTheme();
	}

	SetFrameRounding(m_draw_settings.rounding);

	m_page_list = std::make_unique<CPageList>();
}

CDrawing::~CDrawing()
{
	const auto path = GetLoaderSettingsPath();

	SetValueInt("Theme", "ThemeNum", m_draw_settings.themenum, path.c_str());
	SetValueInt("Theme", "FrameRounding", m_draw_settings.rounding, path.c_str());
	SetValueBool("Inject", "NoCache", g_GlobalVars.injectData.nocache, path.c_str());
	SetValueBool("Inject", "CloseAfterInject", g_GlobalVars.injectData.close, path.c_str());

	m_page_list->Clear();

	DELETE_UNIQUE_PTR(m_page_list);
}

DrawSettings& CDrawing::GetDrawSettings()
{
	return m_draw_settings;
}

void CDrawing::InitPageList()
{
	ImGuiIO& io = ImGui::GetIO();

	m_page_list->Push("Start", [&]()
	{
		if (g_LicenseInfo.bIsUpdatedCheatList)
		{
			g_LicenseInfo.bIsHaveSubscription
				? m_page_list->Swap("General")
				: m_page_list->Swap("HardwareId");
		}
		else
		{
			g_LicenseInfo.bIsAviableUpdate
				? m_page_list->Call("Updating")
				: m_page_list->Call("Loading");
		}
	});

	m_page_list->Push("Loading", [&]()
	{
		const auto window_size = ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT);
		ImGui::Loading(window_size * 0.5f, 10.f, IMAGE_DEFAULT_COLOR, g_GlobalVars.curtime);
		ImGui::Spacing(45);
		ImGui::TextCenter("Loading...");
	});

	m_page_list->Push("Updating", [&]()
	{
		const auto window_size = ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT);
		ImGui::Loading(window_size * 0.5f, 10.f, IMAGE_DEFAULT_COLOR, g_GlobalVars.curtime);
		ImGui::Spacing(45);
		ImGui::TextCenter("Updating...");
	});

	m_page_list->Push("HardwareId", [&]()
	{
		static float copied_time;

		const auto image_size = ImVec2(86, 109);
		const auto button_size = ImVec2(120, 16);
		const auto item_width = 350.f;
		const auto window_width = WINDOW_WIDTH;

		auto& sHardwareId = g_LicenseInfo.sHardwareId;

		ImGui::Spacing(9);
		ImGui::SameLine((window_width - image_size.x) * 0.5f);
		ImGui::Image(g_ImageList[ImageSize::Large].pTexture, image_size, ImVec2(0, 0), ImVec2(1, 1), IMAGE_DEFAULT_COLOR);
		ImGui::Spacing(11);
		ImGui::TextCenter("Your Hardware Identifier:");
		ImGui::Spacing(2);
		ImGui::SameLine((window_width - item_width) * 0.5f);
		ImGui::SetNextItemWidth(item_width);
		ImGui::InputText("##HardwareId", sHardwareId.data(), sHardwareId.size() + 1, ImGuiInputTextFlags_ReadOnly);
		ImGui::Spacing(2);
		ImGui::SameLine((window_width - button_size.x) * 0.5f);

		if (ImGui::Button("Copy to Clipboard", button_size))
		{
			CopyToClipboard(sHardwareId.c_str());
			copied_time = g_GlobalVars.curtime;
		}

		ImGui::Spacing(2);

		if (g_GlobalVars.curtime < copied_time + 2.25f)
		{
			auto alpha = 1.00f;

			if (g_GlobalVars.curtime > copied_time + 2.00f)
				alpha = 1.00f - (g_GlobalVars.curtime - (copied_time + 2.00f)) / 0.25f;

			auto ColorText = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ColorText.w = alpha;

			ImGui::PushStyleColor(ImGuiCol_Text, ColorText);
			ImGui::TextCenter("Hardware ID is copied");
			ImGui::PopStyleColor();
		}
	});

	m_page_list->Push("General", [&]()
	{
		static int current_selected_build;

		const auto button_size = ImVec2(120, 16);
		const auto content_offset_from_start_x = 15.f;
		const auto logo_offset_from_start_x = 40.f;
		const auto window_width = WINDOW_WIDTH;
		const auto window_height = WINDOW_HEIGHT;

		auto list = GetCheatNamesList();
		auto& changelog = g_CheatList[current_selected_build].changelog;

		ImGui::Spacing(3);
		ImGui::SameLine(logo_offset_from_start_x);
		ImGui::BeginGroup();
		ImGui::Image(g_ImageList[ImageSize::Medium].pTexture, ImVec2(72, 88), ImVec2(0, 0), ImVec2(1, 1), IMAGE_DEFAULT_COLOR);
		ImGui::Spacing(2);
		ImGui::SameLine(content_offset_from_start_x - logo_offset_from_start_x);

		if (ImGui::Button("Copy Hardware ID", button_size))
			CopyToClipboard(g_LicenseInfo.sHardwareId.c_str());

		ImGui::Spacing();
		ImGui::SameLine(content_offset_from_start_x - logo_offset_from_start_x);

		if (ImGui::Button("Edit loader theme", button_size))
		{
			ImGui::OpenPopup("Edit loader theme");
			m_draw_settings.modal_opened = true;
		}

		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::VerticalSeparator();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::SetNextItemWidth(250);
		ImGui::ListBox("##CheatList", &current_selected_build, vector_getter, static_cast<void*>(&list), list.size(), 9);
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::PushFont(io.Fonts->Fonts[2]);
		ImGui::Text("Information");
		ImGui::PopFont();
		ImGui::Spacing(2);
		ImGui::TextDisabled("Game: %s", g_CheatList[current_selected_build].game.c_str());
		ImGui::TextDisabled("Version: %s", g_CheatList[current_selected_build].version.c_str());
		ImGui::TextDisabled("Status:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.31f, 0.60f, 0.31f, 1.00f), "Undetected");

		for (int i = 1; i < 4; i++)
		{
			const auto tmp = FindArgument(g_CheatList[current_selected_build].description, i, "^");

			if (tmp.length() < 2)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, io.Fonts->Fonts[0]->FontSize + 4.f));
				ImGui::Spacing();
				ImGui::PopStyleVar();
				continue;
			}

			ImGui::TextDisabled(tmp.c_str());
		}

		ImGui::Spacing();

		if (ImGui::Button("Open settings folder", button_size))
		{
			std::string settings_path;
			settings_path.append(getenv("appdata"));
			settings_path.append("\\");
			settings_path.append(g_CheatList[current_selected_build].folder);
			settings_path.append("\\");
			CreateDirectory(settings_path.c_str(), NULL);
			ShellExecute(0, "open", settings_path.c_str(), 0, 0, SW_RESTORE);
		}

		ImGui::EndGroup();
		ImGui::Spacing(3);
		ImGui::SameLine(content_offset_from_start_x);
		ImGui::BeginGroup();
		ImGui::Separator(window_width - content_offset_from_start_x);
		ImGui::EndGroup();
		ImGui::Spacing(2);
		ImGui::SameLine(content_offset_from_start_x);
		ImGui::BeginGroup();
		ImGui::PushFont(io.Fonts->Fonts[2]);
		ImGui::Text("Instruction");
		ImGui::PopFont();
		ImGui::Spacing(2);
		ImGui::TextDisabled("1. Click the button \"Download & Inject\".");
		ImGui::TextDisabled("2. Wait until the current version of build is downloaded.");
		ImGui::TextDisabled("3. Start the game.");
		ImGui::TextDisabled("4. After the injection you will see a menu of cheat.");
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::VerticalSeparator();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::InputTextMultiline("##ChangeLog", changelog.data(), changelog.size() + 1, ImVec2(360, 76));
		ImGui::EndGroup();
		ImGui::Spacing(2);
		ImGui::SameLine(content_offset_from_start_x);
		ImGui::BeginGroup();
		ImGui::Separator(window_width - content_offset_from_start_x);
		ImGui::EndGroup();
		ImGui::Spacing(2);
		ImGui::SameLine(content_offset_from_start_x);	
		ImGui::BeginGroup();
		ImGui::Spacing(3);

		if (g_GlobalVars.injectData.state == InjectState::None)
		{
			if (ImGui::Button("Download & Inject", button_size))
			{
				g_GlobalVars.injectData.buildnum = current_selected_build;
				g_GlobalVars.injectData.state = InjectState::Download;
				g_GlobalVars.injectData.prevtime = g_GlobalVars.curtime;

				std::thread download_thread(DownloadThread);
				download_thread.detach();
			}
		}
		else
		{
			const auto fadein_duration = 0.500f; // 500 milliseconds
			const auto alpha = (g_GlobalVars.curtime - g_GlobalVars.injectData.prevtime) / fadein_duration;

			auto loading_color = IMAGE_DEFAULT_COLOR;
			auto text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);

			loading_color.w = std::clamp<float>(alpha, 0.0f, 1.0f);
			text_color.w = std::clamp<float>(alpha, 0.0f, 1.0f);

			ImGui::Loading(ImVec2(30, window_height - 47), 8.0f, loading_color, g_GlobalVars.curtime);
			ImGui::Spacing(2);
			ImGui::SameLine(45);
			ImGui::PushStyleColor(ImGuiCol_Text, text_color);

			if (g_GlobalVars.injectData.state == InjectState::Download)
			{
				const double dbSize = g_GlobalVars.dwDownloadedBytes * 0.0000005;
				ImGui::Text("Downloading latest update... (%.1f MB)", dbSize);
			}

			if (g_GlobalVars.injectData.state == InjectState::LookingProcess)
			{
				if (g_GlobalVars.injectData.response.empty())
				{
					ImGui::Text("Error. Retrying to download...");

					if (g_GlobalVars.injectData.prevtime + 2.5f < g_GlobalVars.curtime)
					{
						g_GlobalVars.injectData.state = InjectState::Download;

						std::thread download_thread(DownloadThread);
						download_thread.detach();
					}
				}
				else
				{
					ImGui::Text("Waiting for game launch...");

					if (g_GlobalVars.injectData.processId)
					{
						g_GlobalVars.injectData.state = InjectState::Processing;
						g_GlobalVars.injectData.prevtime = g_GlobalVars.curtime;
					}
					else
					{
						static float found_process_delay;

						if (found_process_delay + 2.5f < g_GlobalVars.curtime)
						{
							std::thread looking_process_thread(LookingProcessThread);
							looking_process_thread.detach();
							found_process_delay = g_GlobalVars.curtime;
						}
					}
				}
			}

			if (g_GlobalVars.injectData.state == InjectState::Processing)
			{
				ImGui::Text("Injecting...");

				if (!g_GlobalVars.injectData.injecting)
				{
					std::thread injecting_thread(InjectingThread);
					injecting_thread.detach();
					g_GlobalVars.injectData.injecting = true;
				}
			}

			ImGui::PopStyleColor();
		}

		ImGui::Spacing(3);
		ImGui::EndGroup();
		ImGui::SameLine(315);
		ImGui::VerticalSeparator();
		ImGui::SameLine(324);
		ImGui::BeginGroup();
		ImGui::Checkbox("Don't create cache file", &g_GlobalVars.injectData.nocache);
		ImGui::Checkbox("Close loader after inject", &g_GlobalVars.injectData.close);
		ImGui::EndGroup();

		if (m_draw_settings.modal_opened)
		{
			const auto modal_size = ImVec2(280, 330);
			const auto modal_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	
			ImGui::SetNextWindowSize(modal_size);
			ImGui::PushFont(io.Fonts->Fonts[2]);

			if (ImGui::BeginPopupModal("Edit loader theme", &m_draw_settings.modal_opened, modal_flags))
			{
				ImGui::PushFont(io.Fonts->Fonts[0]);
				ImGui::Spacing(3);
				ImGui::SameLine(8);
				ImGui::BeginGroup();
				ImGui::Text("Color theme:");
				ImGui::Spacing();

				if (ImGui::RadioButton("White", &m_draw_settings.themenum, WhiteTheme))
					SetWhiteTheme();

				if (ImGui::RadioButton("Dark", &m_draw_settings.themenum, DarkTheme))
					SetDarkTheme();

				ImGui::Spacing(3);

				if (ImGui::SliderInt("Rounding", &m_draw_settings.rounding, 0, 7, "%d"))
					SetFrameRounding(m_draw_settings.rounding);

				ImGui::EndGroup();
				ImGui::PopFont();

				if (io.KeysDown[VK_ESCAPE])
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			ImGui::PopFont();
		}
	});

	m_page_list->Swap("Start");
}

void CDrawing::Run()
{
	ImGuiIO& io = ImGui::GetIO();

	const char* window_name = "##Main";
	const float window_width = WINDOW_WIDTH;
	const float window_height = WINDOW_HEIGHT;
	const auto  window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

	if (m_page_list->Empty())
		InitPageList();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(window_width, window_height));

	ImGui::Begin(window_name, nullptr, window_flags);
	ImGui::Spacing(2);
	ImGui::SameLine(8);
	ImGui::BeginGroup();
	ImGui::Image(g_ImageList[ImageSize::Small].pTexture, ImVec2(17, 20), ImVec2(0, 0), ImVec2(1, 1), IMAGE_DEFAULT_COLOR);
	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::VerticalSeparator();
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::Spacing();
	ImGui::PushFont(io.Fonts->Fonts[2]);
	ImGui::Text("Hpp Loader");
	ImGui::PopFont();
	ImGui::EndGroup();

	if (ImGui::CloseButton(ImGui::GetID(window_name), ImVec2(window_width - 14.f, 14.f), 9.0f))
		g_GlobalVars.bShutdown = true;

	ImGui::BeginChild("##Elements", ImVec2(0, window_height - 48.f), true);

	m_page_list->Run();

	ImGui::EndChild();
	ImGui::Spacing();
	ImGui::SameLine(8);
	ImGui::Text(u8"© 2019 HPP. All rights reserved.");
	ImGui::SameLine();
	ImGui::End();
}

std::vector<std::string> GetCheatNamesList()
{
	std::vector<std::string> list;

	for (auto& cheat : g_CheatList)
		list.push_back(cheat.name);

	return list;
}

bool vector_getter(void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);

	if (idx < 0 || idx >= static_cast<int>(vector.size()))
		return false;

	*out_text = vector.at(idx).c_str();
	return true;
};

void SetWhiteTheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.98f, 0.99f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.08f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.29f, 0.29f, 0.29f, 0.52f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.39f, 0.39f, 0.8f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.08f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.32f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.08f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.07f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.00f, 0.00f, 0.08f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
}

void SetDarkTheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.29f, 0.29f, 0.29f, 0.52f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.39f, 0.39f, 0.8f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.32f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
	style.Colors[ImGuiCol_Column] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.29f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
}

void SetFrameRounding(int value)
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = static_cast<float>(value);
}