#include "..\menu_consts.h"
#include "menu_tabs.h"

#include "..\..\..\cvars.h"

void CMenuTabs::TabVisuals()
{
	BeginTab(&cvar.visuals.active, "Active##Visuals");
	BeginColumn();

	BeginChild("Player##Visuals", ImVec2(GetChildWidth(), 253));
	PushId("Visuals.Player");
	Checkbox(&cvar.visuals.esp_player_enabled, "Enabled", cvar.settings.tooltip_language ? u8"Глобальное включение/выключение ESP на игроках." : "Global player ESP on/off.");
	PushCondition(cvar.visuals.esp_player_enabled);
	Checkbox(&cvar.visuals.esp_player_enemies, "Enemies", cvar.settings.tooltip_language ? u8"Отображать ESP на врагах." : "Display ESP on enemies.");
	Checkbox(&cvar.visuals.esp_player_teammates, "Teammates", cvar.settings.tooltip_language ? u8"Отображать ESP на товарищах по команде." : "Display ESP on teammates.");
	Checkbox(&cvar.visuals.esp_player_visible_check, "Visible check", cvar.settings.tooltip_language ? u8"Проверка на видимость игрока." : "Check player visibility.");
	Combo(&cvar.visuals.esp_player_box, "Bounding box", IM_ARRAYCOMBO(esp_player_box), cvar.settings.tooltip_language ? u8"Рисовать боксы на игроках." : "Draw boxes on players.");
	PushCondition(cvar.visuals.esp_player_box);
	Checkbox(&cvar.visuals.esp_player_box_outline, "Bounding box outline", cvar.settings.tooltip_language ? u8"Рисовать контур на боксах." : "Draw the outline on the boxes.");
	SliderInt(&cvar.visuals.esp_player_box_thickness, "Bounding box thickness", 1, 3, "%.0f pixels", cvar.settings.tooltip_language ? u8"Толщина линий боксов." : "Box line thickness.");
	Text("Bounding box colors hide");
	ColorButton4(cvar.visuals.esp_player_box_color_hide_t, "##TerroristColorHide"); ImGui::SameLine();
	Text("T");
	ColorButton4(cvar.visuals.esp_player_box_color_hide_ct, "##CounterTerroristColorHide", 0.f); ImGui::SameLine();
	Text("CT");
	Text("Bounding box colors visible");
	ColorButton4(cvar.visuals.esp_player_box_color_visible_t, "##TerroristColorVisible"); ImGui::SameLine();
	Text("T");
	ColorButton4(cvar.visuals.esp_player_box_color_visible_ct, "##CounterTerroristColorVisible", 0.f); ImGui::SameLine();
	Text("CT");	
	PopCondition();
	Combo(&cvar.visuals.esp_player_name, "Name", IM_ARRAYCOMBO(esp_position), cvar.settings.tooltip_language ? u8"Отображать никнеймы игроков." : "Display player nicknames.");
	Combo(&cvar.visuals.esp_player_weapon, "Weapon", IM_ARRAYCOMBO(esp_position), cvar.settings.tooltip_language ? u8"Отображать оружия игроков." : "Display player weapons.");
	PushCondition(cvar.visuals.esp_player_weapon);
	Checkbox(&cvar.visuals.esp_player_weapon_ammo, "Weapon ammo", cvar.settings.tooltip_language ? u8"Отображать количество патронов оружия у игроков." : "Display player weapon ammo.");
	PopCondition();
	Combo(&cvar.visuals.esp_player_distance, "Distance", IM_ARRAYCOMBO(esp_position), cvar.settings.tooltip_language ? u8"Отображать дистанцию до игроков." : "Display distance to players.");
	PushCondition(cvar.visuals.esp_player_distance);
	Combo(&cvar.visuals.esp_player_distance_measure, "Distance measurement", IM_ARRAYCOMBO(esp_player_distance), cvar.settings.tooltip_language ? u8"Измерение расстояния до игроков." : "The measurement of the distance to the players.");
	PopCondition();
	Combo(&cvar.visuals.esp_player_health, "Health bar", IM_ARRAYCOMBO(esp_position), cvar.settings.tooltip_language ? u8"Рисовать количество здоровья игроков на баре." : "Draw a player health bar.");
	Combo(&cvar.visuals.esp_player_armor, "Armor bar", IM_ARRAYCOMBO(esp_position), cvar.settings.tooltip_language ? u8"Рисовать количество брони игроков на баре." : "Draw a player armor bar.");
	Checkbox(&cvar.visuals.esp_player_skeleton, "Skeleton", cvar.settings.tooltip_language ? u8"Рисовать скелет на игроках." : "Draw the skeleton on the players.");
	PushCondition(cvar.visuals.esp_player_skeleton);
	ColorButton4(cvar.visuals.esp_player_skeleton_color, "##SkeletonColor", 0.f);
	PopCondition();
	Combo(&cvar.visuals.esp_player_out_of_pov, "Out of POV", IM_ARRAYCOMBO(esp_out_of_pow), cvar.settings.tooltip_language ? u8"Рисовать позиции игроков, находящихся вне поле зрения." : "Draw location positions for players out of sight.");
	PushCondition(cvar.visuals.esp_player_out_of_pov);
	SliderFloat(&cvar.visuals.esp_player_out_of_pov_radius, "Out of POV radius", 0.1f, 1.f, "%.2f", 1.f, cvar.settings.tooltip_language ? u8"Радиус отрисовки позиции игроков, находящихся вне поле зрения." : "The radius of the drawing position, out of sight.");
	PopCondition();
	SliderFloat(&cvar.visuals.esp_player_history_time, "History time", 0.f, 2.5f, "%.1f sec", 1.f, cvar.settings.tooltip_language ? u8"Время отображения визуалов после того, как состояние валидации игрока стало ложное." : "Time to display visuals after a player's state is false.");
	Checkbox(&cvar.visuals.esp_player_fadeout_dormant, "Fade out dormant", cvar.settings.tooltip_language ? u8"Плавное исчезание визуалов после того, как состояние валидации игрока стало ложное." : "Smooth fading of visuals after a player’s state is false.");
	Checkbox(&cvar.visuals.esp_sound_enabled, "Sound", cvar.settings.tooltip_language ? u8"Рисовать звуковое ESP." : "Draw sound ESP.");
	Combo(&cvar.visuals.esp_sound_draw_type, "Sound draw type", IM_ARRAYCOMBO(esp_sound), cvar.settings.tooltip_language ? u8"Тип отрисовки звукового ESP." : "Type of drawing sound ESP.");	
	PushCondition(cvar.visuals.esp_sound_enabled);
	SliderFloat(&cvar.visuals.esp_sound_size, "Sound size", 0.f, 25.f, "%.1f", 1.f, cvar.settings.tooltip_language ? u8"Размер квадратов/кругов звукового ESP." : "The size of boxes/circles sound ESP.");
	SliderFloat(&cvar.visuals.esp_sound_fadeout_time, "Sound fade out time", 0.f, 2.5f, "%.1f sec", 1.f, cvar.settings.tooltip_language ? u8"Время плавного исчезновения звукового ESP." : "Fade out time of sound ESP.");
	Text("Sound colors");
	ColorButton4(cvar.visuals.esp_sound_color_t, "##TerroristSoundColor"); ImGui::SameLine();
	Text("T");
	ColorButton4(cvar.visuals.esp_sound_color_ct, "##CounterTerroristSoundColor", 0.f); ImGui::SameLine();
	Text("CT");
	PopCondition();
	SliderFloat(&cvar.visuals.esp_font_size, "Font size", 0.1f, 18.f, "%.1f px", 1.f, cvar.settings.tooltip_language ? u8"Размер отображаемого шрифта." : "The size of the displayed font.");
	Combo(&cvar.visuals.esp_font_outline, "Font outline", IM_ARRAYCOMBO(font_outline), cvar.settings.tooltip_language ? u8"Рисовать контур на шрифте." : "Draw the outline on the font.");
	Text("Font color");
	ColorButton4(cvar.visuals.esp_font_color, "##FontColor");
	PopCondition();
	EndChild();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("Colored models##Visuals", ImVec2(GetChildWidth(), 177));
	PushId("Visuals.Colored models");
	EndChild();

	NextColumn();

	BeginChild("Other##Visuals", ImVec2(GetChildWidth(), 217));
	SliderInt(&cvar.visuals.max_render_calls, "Max render calls per second", 1, 300, "%.0f"); // desc
	//Checkbox(&cvar.visuals.shared_esp, "Shared ESP");
	EndChild();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("Effects##Visuals", ImVec2(GetChildWidth(), 213));
	PushId("Visuals.Effects");
	EndChild();

	EndColumn();
	EndTab();
}