#pragma once
#include "Menu.h"
#include "../HanaLovesMe/HanaLovesMe.h"

HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();

// =====================================
// - Custom controls
// =====================================

#define InsertSpacer(x1) style->Colors[HanaLovesMeCol_ChildBg] = xuicolor(0, 0, 0, 0); HanaLovesMe::BeginChild(x1, vsize(210.f, 18.f), false); {} HanaLovesMe::EndChild(); style->Colors[HanaLovesMeCol_ChildBg] = xuicolor(49, 49, 49, 255);
#define InsertGroupboxSpacer(x1) style->Colors[HanaLovesMeCol_ChildBg] = xuicolor(0, 0, 0, 0); HanaLovesMe::BeginChild(x1, vsize(210.f, 9.f), false); {} HanaLovesMe::EndChild(); style->Colors[HanaLovesMeCol_ChildBg] = xuicolor(49, 49, 49, 255);
#define InsertGroupboxTitle(x1) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(11.f); HanaLovesMe::GroupBoxTitle(x1);

#define InsertGroupBoxLeft(x1,x2) HanaLovesMe::NewLine(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBox(x1, vsize(258.f, x2), true);
#define InsertGroupBoxCentered(x1,x2) HanaLovesMe::NewLine(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBox(x1, vsize(535.f, x2), true);
#define InsertGroupBoxRight(x1,x2) HanaLovesMe::NewLine(); HanaLovesMe::SameLine(10.f); HanaLovesMe::BeginGroupBox(x1, vsize(258.f, x2), true);
#define InsertEndGroupBoxLeft(x1,x2) HanaLovesMe::EndGroupBox(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBoxScroll(x1, x2, vsize(258.f, 11.f), true); HanaLovesMe::EndGroupBoxScroll();
#define InsertEndGroupBoxCentered(x1,x2) HanaLovesMe::EndGroupBox(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBoxScroll(x1, x2, vsize(526.f, 11.f), true); HanaLovesMe::EndGroupBoxScroll();
#define InsertEndGroupBoxRight(x1,x2) HanaLovesMe::EndGroupBox(); HanaLovesMe::SameLine(10.f); HanaLovesMe::BeginGroupBoxScroll(x1, x2, vsize(258.f, 11.f), true); HanaLovesMe::EndGroupBoxScroll();

#define InsertGroupBoxTop(x1,x2) HanaLovesMe::NewLine(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBox(x1, x2, true);
#define InsertEndGroupBoxTop(x1,x2,x3) HanaLovesMe::EndGroupBox(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBoxScroll(x1, x2, x3, true); HanaLovesMe::EndGroupBoxScroll();

// v2

#define InsertGroupBoxLeft2(x1,x2) HanaLovesMe::NewLine(); HanaLovesMe::SameLine(19.f); HanaLovesMe::BeginGroupBox2(x1, vsize(258.f, x2), true);
#define InsertGroupBoxRight2(x1,x2) HanaLovesMe::NewLine(); HanaLovesMe::SameLine(10.f); HanaLovesMe::BeginGroupBox2(x1, vsize(258.f, x2), true);

// =====================================
// - Default controls
// =====================================

#define InsertCheckbox(x1,x2) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(19.f); HanaLovesMe::Checkbox(x1, &x2);
#define InsertButton(x1) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::Button(x1);
//#define InsertButton(x1,x2) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);HanaLovesMe:Button(x1, &x2);// HanaLovesMe::PopItemWidth(); HanaLovesMe::CustomSpacing(1.f);
#define InsertSlider(x1,x2,x3,x4,x5) HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f); HanaLovesMe::SliderFloat(x1, &x2, x3, x4, x5); HanaLovesMe::PopItemWidth();// was 159
#define InsertSliderWithoutText(x1,x2,x3,x4,x5) HanaLovesMe::Spacing(); HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(159.f); HanaLovesMe::SliderFloat(x1, &x2, x3, x4, x5); HanaLovesMe::PopItemWidth();
#define InsertCombo(x1,x2,x3) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f); HanaLovesMe::Combo(x1, &x2, x3, IM_ARRAYSIZE(x3)); HanaLovesMe::PopItemWidth(); HanaLovesMe::CustomSpacing(1.f);
#define InsertComboWithoutText(x1,x2,x3) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f); HanaLovesMe::Combo(x1, &x2, x3, IM_ARRAYSIZE(x3)); HanaLovesMe::PopItemWidth(); HanaLovesMe::CustomSpacing(1.f);
#define InsertMultiCombo(x1,x2,x3,x4) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f); HanaLovesMe::MultiCombo(x1, x2, x3, x4); HanaLovesMe::PopItemWidth(); HanaLovesMe::CustomSpacing(1.f);
#define InsertMultiComboWithoutText(x1,x2,x3,x4) HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f); HanaLovesMe::MultiCombo(x1, x2, x3, x4); HanaLovesMe::PopItemWidth(); HanaLovesMe::CustomSpacing(1.f);

#define InsertColorPicker(x1,x2,x3) HanaLovesMe::SameLine(219.f); Menu::ColorPicker(x1, x2, x3);
