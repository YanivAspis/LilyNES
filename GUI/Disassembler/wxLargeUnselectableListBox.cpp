#include "wxLargeUnselectableListBox.h"

wxBEGIN_EVENT_TABLE(wxLargeUnselectableListBox, wxScrolledWindow)
	EVT_SCROLLWIN(wxLargeUnselectableListBox::OnScroll)
	EVT_ERASE_BACKGROUND(wxLargeUnselectableListBox::OnBackgroundErase)
wxEND_EVENT_TABLE()


wxLargeUnselectableListBox::wxLargeUnselectableListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) 
	: wxScrolledWindow(parent, id, pos, size, wxVSCROLL | wxFULL_REPAINT_ON_RESIZE | wxRETAINED)
{
	Bind(wxEVT_PAINT, &wxLargeUnselectableListBox::OnPaint, this);
	Bind(wxEVT_SIZE, &wxLargeUnselectableListBox::OnSize, this);
	SetBackgroundColour(*wxWHITE);

	m_fontSize = 0;
	m_marginFromLeft = 5;
	m_marginBetweenLines = 0;
	m_scrollRate = 0;

	m_selectedItem = -1;
}

void wxLargeUnselectableListBox::AppendItem(wxString item) {
	m_items.Add(item);
	this->UpdateScrollbars();
}

void wxLargeUnselectableListBox::InsertItem(wxString item, size_t index) {
	m_items.Insert(item, index);
	this->UpdateScrollbars();
}

void wxLargeUnselectableListBox::SetItems(wxArrayString& items) {
	m_items = items;
	this->UpdateScrollbars();
}

void wxLargeUnselectableListBox::InsertItems(wxArrayString& items, size_t index) {
	m_items.insert(m_items.begin() + index, items.begin(), items.end());
	this->UpdateScrollbars();
}

void wxLargeUnselectableListBox::RemoveItems(size_t index, size_t count) {
	m_items.RemoveAt(index, count);
	this->UpdateScrollbars();
}

void wxLargeUnselectableListBox::Clear() {
	m_items.Clear();
	this->UpdateScrollbars();
}

void wxLargeUnselectableListBox::SelectItem(size_t index) {
	if (index < 0 && index >= m_items.Count()) {
		throw std::runtime_error("Index out of range");
	}
	m_selectedItem = index;
	int y = (m_selectedItem * (m_fontSize + m_marginBetweenLines) - this->GetClientSize().GetHeight() / 2) / m_scrollRate;
	y = std::max(y, 0);
	this->Scroll(wxPoint(0, y));
	this->Refresh();
}

void wxLargeUnselectableListBox::OnPaint(wxPaintEvent& evt) {

	wxBufferedPaintDC dc(this);
	this->DoPrepareDC(dc);
	wxSize size = this->GetClientSize();

	int unscrolledY = this->GetViewStart().y * m_scrollRate;
	int unscrolledHeight = unscrolledY + size.GetHeight();

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(dc.GetBackground());
	dc.DrawRectangle(wxRect(0, unscrolledY, size.GetWidth(), unscrolledHeight));

	// Draw border
	dc.SetPen(wxPen(*wxBLACK));


	// Draw items
	dc.SetFont(wxFont(wxFontInfo(m_fontSize)));
	int itemHeight = m_fontSize + m_marginBetweenLines;

	int i = unscrolledY / itemHeight;
	float y = unscrolledY;
	while ((y < unscrolledHeight) && i < m_items.Count()) {
		if (i == m_selectedItem) {
			dc.SetFont(wxFont(wxFontInfo(m_fontSize).Bold()));
			dc.DrawText(m_items[i], wxPoint(m_marginFromLeft, y));
			dc.SetFont(wxFont(wxFontInfo(m_fontSize)));
		}
		else {
			dc.DrawText(m_items[i], wxPoint(m_marginFromLeft, y));
		}
		i++;
		y += itemHeight;
	}

	evt.Skip();
}

void wxLargeUnselectableListBox::OnSize(wxSizeEvent& evt) {
	wxSize newSize = evt.GetSize();
	m_fontSize = std::min(newSize.GetHeight() / 20, newSize.GetWidth() / 20);
	m_marginBetweenLines = m_fontSize / 2;
	this->UpdateScrollbars();
	evt.Skip();
}

void wxLargeUnselectableListBox::OnScroll(wxScrollWinEvent& evt) {
	this->Refresh();
	evt.Skip();
}

void wxLargeUnselectableListBox::UpdateScrollbars() {
	wxSize virtualSize = this->GetVirtualSize();
	int virtualY = m_items.Count() * (m_fontSize + m_marginBetweenLines);
	this->SetVirtualSize(this->GetClientSize().GetWidth(), virtualY);
	m_scrollRate = m_fontSize;
	this->SetScrollRate(0, m_scrollRate);
}