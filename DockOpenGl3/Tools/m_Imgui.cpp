#include "m_Imgui.h"

void m_Imgui_Combo(std::string CombomName,std::vector<std::string>& Vec, int& SelectNum)
{
	const char* combo_preview_value = Vec[SelectNum].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo(CombomName.c_str(), combo_preview_value, 0))
	{
		for (int n = 0; n < Vec.size(); n++)
		{
			const bool is_selected = (SelectNum == n);
			if (ImGui::Selectable(Vec[n].c_str(), is_selected))
				SelectNum = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}
