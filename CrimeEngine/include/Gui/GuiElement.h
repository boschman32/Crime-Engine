#pragma once

#include "Gameplay/GameObject.h"
#include "GuiHelper.h"
#include "Core/Math.h"

#include <memory>
#include <vector>

namespace ce { class IGuiLayoutModifier; }

namespace ce
{
	class GuiElement
	{
	public:
		GuiElement(GuiElement* a_parent);
		virtual ~GuiElement();

		void OnLayout();
		virtual void OnLayoutChange();
		virtual void OnDraw(cr::RenderLayer& a_renderLayer);

		//Tip: Use pivot::- TopLeft, TopCenter, TopRight, MiddleLeft, MiddleCenter, MiddleRight, BottomLeft, BottomCenter, BottomRight.
		void SetPivot(const Vector2& a_pivot);
		const Vector2& GetPivot() const { return m_pivotPoint; };
		//Tip: Use anchor::- TopLeft, TopCenter, TopRight, MiddleLeft, MiddleCenter, MiddleRight, BottomLeft, BottomCenter, BottomRight, TopStretch, MiddleStretch, BottomStretch, StretchLeft, StretchCenter, StretchRight, StretchStretch.
		//(Optional: Update the pivot to the same point.)
		void SetAnchor(const Vector4& a_anchor);
		const Vector4& GetAnchor() const { return m_anchor; };
		//Tip: The values might be offset/inset (or a combination), this is based upon the anchor rect.
		//Offset: X, Y, Width, Height.
		//Inset: Left, Top, Right, Bottom.
		//Horizontal: Left, Y, Right, Height.
		//Vertical: X, Top, Width, Bottom.
		void SetRect(float a_xl, float a_yt, float a_wr, float a_hb);
		void SetRect(const Vector4& a_rect);
		const Vector4& GetRect() const { return m_rect; };

		const Vector2& GetSize() const { return m_size; };
		const Vector2& GetPosition() const { return m_position; };
		const Vector2& GetPivotOffset() const { return m_pivotOffset; };
		const Vector2& GetRectOffsetMin() const { return m_rectOffsetMin; };
		const Vector2& GetRectOffsetMax() const { return m_rectOffsetMax; };

		void AddLayoutModifier(IGuiLayoutModifier& a_guiLayoutModifier);
		void RemoveLayoutModifier(IGuiLayoutModifier& a_guiLayoutModifier);

		void SetDirty();
		const bool& IsDirty() const { return m_isDirty; };
		void SetActive(bool a_active);
		const bool& IsActive() const { return m_isActive; };

		const PointType& GetXWLR() const { return m_xwlr; };
		const PointType& GetYHTB() const { return m_yhtb; };

		void GetChildren(std::vector<GuiElement*>& a_children);
		void SetParent(GuiElement& a_parent);
		const GuiElement* GetParent() const { return m_parent; };
		const GuiElement* GetCanvas() const;

		void UpdateValues();

		GuiInteraction m_interaction;
	protected:
		//TODO: Remove temp fix for Canvas updates.
		const bool& AnyDirt() const { return ms_anyDirt; };
		void ClearDirt() const;

		//Custom layout override modifiers.
		std::vector<IGuiLayoutModifier*> m_modifiers = std::vector<IGuiLayoutModifier*>();
		//Elements that are attached to this.
		std::vector<GuiElement*> m_children = std::vector<GuiElement*>();
		//The element it is attached to.
		GuiElement* m_parent = nullptr;
		mutable const GuiElement* m_canvas = nullptr;

		//TODO: Remove temp fix for Gui sorting.
		static float ms_offsetZ;

	private:
		void AddChild(GuiElement& a_child);
		void RemoveChild(const GuiElement& a_child);

		void CalculateSize();
		void CalculatePosition();
		void CalculatePivotOffset();
		void CalculateRectOffsetMin();
		void CalculateRectOffsetMax();

		PointType m_xwlr = PointType::Offset;
		PointType m_yhtb = PointType::Offset;

		Vector4 m_rect{ 0, 0, 0, 0 };
		Vector2 m_pivotPoint{ 0, 0 };
		Vector4 m_anchor{ 0, 0, 1, 1 };

		bool m_isDirty = false;
		bool m_isActive = true;

		//Calculated data.
		Vector2 m_size{ 0, 0 };
		Vector2 m_position{ 0, 0 };
		Vector2 m_pivotOffset{ 0, 0 };
		Vector2 m_rectOffsetMin{ 0, 0 };
		Vector2 m_rectOffsetMax{ 0, 0 };

		//TODO: Remove temp fix for Canvas updates.
		static bool ms_anyDirt;
	};
}