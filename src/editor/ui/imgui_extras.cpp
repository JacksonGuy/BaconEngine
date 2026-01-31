#include "imgui_extras.h"

#include <string_view>

#include "imgui.h"
#include "imgui_internal.h"

static ImVec2 Subtract(const ImVec2 a, const ImVec2 b) {
	return ImVec2{b.x - a.x,b.y - a.y};
}

void ImGui::ItemLabel(std::string_view title, ItemLabelFlag flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImVec2 lineStart = ImGui::GetCursorScreenPos();
	const ImGuiStyle& style = ImGui::GetStyle();
	float fullWidth = ImGui::GetContentRegionAvail().x;
	float itemWidth = ImGui::CalcItemWidth() + style.ItemSpacing.x;
	ImVec2 textSize = ImGui::CalcTextSize(title.data(), title.data()+title.length());
	ImRect textRect;
	textRect.Min = ImGui::GetCursorScreenPos();
	if(flags & ItemLabelFlag::Right)
		textRect.Min.x = textRect.Min.x + itemWidth;
	textRect.Max = textRect.Min;
	textRect.Max.x += fullWidth - itemWidth;
	textRect.Max.y += textSize.y;

	ImGui::SetCursorScreenPos(textRect.Min);

	ImGui::AlignTextToFramePadding();
	// Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
	textRect.Min.y += window->DC.CurrLineTextBaseOffset;
	textRect.Max.y += window->DC.CurrLineTextBaseOffset;

	ImGui::ItemSize(textRect);
	if(ImGui::ItemAdd(textRect, window->GetID(title.data(), title.data() + title.size()))) {
		ImGui::RenderTextEllipsis(
            ImGui::GetWindowDrawList(), 
            textRect.Min, 
            textRect.Max, 
			textRect.Max.x, 
            title.data(), 
            title.data() + title.size(), 
            &textSize
        );

		if(textRect.GetWidth() < textSize.x && ImGui::IsItemHovered())
			ImGui::SetTooltip("%.*s", (int)title.size(), title.data());
	}
	if(flags & ItemLabelFlag::Left) {
		ImGui::SetCursorScreenPos(Subtract(textRect.Max, ImVec2{0, textSize.y + window->DC.CurrLineTextBaseOffset}));
		ImGui::SameLine();
	} else if(flags & ItemLabelFlag::Right)
		ImGui::SetCursorScreenPos(lineStart);
}
