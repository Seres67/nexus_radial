//
// Created by Seres67 on 19/05/2024.
//

#include "addon/Wheel.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

Wheel::Wheel(const char *name, Key key) : m_name(_strdup(name)), m_key(key) {

}

void Wheel::add_element(mts_action action) {
    m_elements.emplace_back(action);
}

void Wheel::open_wheel() {
    ImGui::OpenPopup((std::string(m_name) + "##" + m_name + "_wheel").c_str());
    m_wheel_open = true;
}

void Wheel::close_wheel() {
    ImGui::CloseCurrentPopup();
    m_wheel_open = false;
}

bool Wheel::is_open() const {
    return m_wheel_open;
}

bool Wheel::render_wheel() {
    if (ImGui::BeginPopup((std::string(m_name) + "##" + m_name + "_wheel").c_str(), ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
        const ImVec2 center = ImGui::GetMousePosOnOpeningCurrentPopup();
        int p_selected = -1;
        const ImVec2 drag_delta = ImVec2(ImGui::GetIO().MousePos.x - center.x, ImGui::GetIO().MousePos.y - center.y);
        const float drag_dist2 = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;
        const ImGuiStyle &style = ImGui::GetStyle();
        const float RADIUS_MIN = 90.0f;
        const float RADIUS_MAX = 360.0f;
        const float RADIUS_INTERACT_MIN = 80.0f;    // Handle hit testing slightly below RADIUS_MIN

        // Draw background
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRectFullScreen();
        // um, the circumference of a circle is 2*PI*R, wtf imgui? PI*2 only works if center.x is greater than some unknown value
        draw_list->PathArcTo(center, (RADIUS_MIN + RADIUS_MAX) * 0.5f, 0.0f, IM_PI * 2.3f, 64);
        draw_list->PathStroke(IM_COL32(0, 0, 0, 255), true, RADIUS_MAX - RADIUS_MIN);

        const float item_arc_span = 2 * IM_PI / ImMax((int) m_elements.size(), 6);
        float drag_angle = ImAtan2(drag_delta.y, drag_delta.x);
        if (drag_angle < -0.5f * item_arc_span)
            drag_angle += 2.0f * IM_PI;
        //ImGui::Text("%f", drag_angle);    // [Debug]

        // Draw items
        m_hovered_id = -1;
        for (int item_id = 0; item_id < m_elements.size(); ++item_id) {
            std::string item_label = m_elements[item_id].action_name;
            const float item_ang_min = item_arc_span * (item_id + 0.02f) -
                                       item_arc_span * 0.5f; // FIXME: Could calculate padding angle based on how many pixels they'll take
            const float item_ang_max = item_arc_span * (item_id + 0.98f) - item_arc_span * 0.5f;
            bool hovered = false;
            if (drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN)
                if (drag_angle >= item_ang_min && drag_angle < item_ang_max)
                    hovered = true;
            bool selected = (p_selected == item_id);

            draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max);
            draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min);
            draw_list->PathFillConvex(ImGui::GetColorU32(hovered ? ImGuiCol_HeaderHovered : selected ? ImGuiCol_HeaderActive : ImGuiCol_Header));

            ImVec2 text_size = ImGui::CalcTextSize(item_label.c_str());
            ImVec2 text_pos = ImVec2(
                    center.x + cosf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.x * 0.5f,
                    center.y + sinf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.y * 0.5f);
            draw_list->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), item_label.c_str());

            if (hovered)
                m_hovered_id = item_id;
        }
        draw_list->PopClipRect();
        return true;
    }
    return false;
}

const char *Wheel::get_wheel_name() const {
    return m_name;
}

std::vector<mts_action> Wheel::get_elements() {
    return m_elements;
}

mts_action Wheel::get_hovered_element() const {
    if (m_hovered_id != -1)
        return m_elements[m_hovered_id];
    return {};
}

Key Wheel::get_key() const {
    return m_key;
}
