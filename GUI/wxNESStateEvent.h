#pragma once

#include "wx/event.h"

template<typename T> class wxNESStateEvent;

template<typename T>
class wxNESStateEvent : public wxCommandEvent {
public:
	wxNESStateEvent(wxEventType eventType, int id = 0);
	wxNESStateEvent(const wxNESStateEvent& evt);

	wxEvent* Clone() const;

	void SetState(const T& state);
	T GetState() const;

private:
	T m_state;
};

template <typename T>
wxNESStateEvent<T>::wxNESStateEvent(wxEventType eventType, int id) : wxCommandEvent(eventType, id) {}

template <typename T>
wxNESStateEvent<T>::wxNESStateEvent(const wxNESStateEvent& evt) : wxCommandEvent(evt)
{
	this->SetState(evt.GetState());
}

template <typename T>
wxEvent* wxNESStateEvent<T>::Clone() const
{
	return new wxNESStateEvent<T>(*this);
}

template <typename T>
void wxNESStateEvent<T>::SetState(const T& state)
{
	m_state = state;
}

template <typename T>
T wxNESStateEvent<T>::GetState() const
{
	return m_state;
}
