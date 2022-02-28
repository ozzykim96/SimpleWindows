#include "windows.h"

class MessageWnd : public Window
{
public:
	MessageWnd(Window* listener, std::string title, std::string msg, MessageWindowButtonStyle style, std::string strOK, std::string strCancel)
		: m_listener(listener), m_ok(strOK), m_cancel(strCancel)
	{
		Rect scrn = WindowManager::getManager()->getScreen()->getClientRect();
		Rect rc;

		rc.set(scrn.getWidth() / 4, scrn.getHeight() / 3,
			scrn.getWidth() * 3 / 4, scrn.getHeight() * 2 / 3);

		create(NULL, rc, WS_VISIBLE);

		m_title.create(this, Rect(0, 0, rc.getWidth(), rc.getHeight() / 3), WS_VISIBLE);
		m_title.setText(title);

		m_msg.create(this, Rect(0, rc.getHeight() / 3, rc.getWidth(), rc.getHeight() * 2 / 3), WS_VISIBLE);
		m_msg.setText(msg);

		switch (style)
		{
		case MSGWNDBTNSTYLE_OK:
			m_ok.create(this, Rect(rc.getWidth() / 3, rc.getHeight() * 2 / 3, rc.getWidth() * 2 / 3, rc.getHeight()), WS_VISIBLE);
			m_ok.setText("OK");
			break;
		case MSGWNDBTNSTYLE_OKCANCEL:
			m_ok.create(this, Rect(0, rc.getHeight() * 2 / 3, rc.getWidth() / 2, rc.getHeight()), WS_VISIBLE);
			if( m_ok.getText().size() == 0 )
			{
				m_ok.setText("OK");
			}

			m_cancel.create(this, Rect(rc.getWidth() / 2, rc.getHeight() * 2 / 3, rc.getWidth(), rc.getHeight()), WS_VISIBLE);
			if( m_cancel.getText().size() == 0 )
			{
				m_cancel.setText("CANCEL");
			}
			break;
		}

		setFocusChild(&m_ok);

		m_active = WindowManager::getManager()->setFocus(this);
	}

	unsigned int onCommand(Window* wnd, unsigned int event, unsigned int param1, unsigned int param2)
	{
		if (event == WE_CLICK)
		{
			if (wnd == &m_ok)
			{
				WindowManager::getManager()->setFocus(m_active);
				// send
				onSelect(MSGWNDBTN_OK);
				delete this;
			}
			else
			{
				WindowManager::getManager()->setFocus(m_active);
				// send
				onSelect(MSGWNDBTN_CANCEL);
				delete this;
			}
		}
		return 0;
	}

	virtual void onSelect(MessageWindowButton btn)
	{
		// send WE_SELECT to the parent.
		if (m_listener)
		{
			send(m_listener, WE_SELECT, btn, 0);
		}
	}

	virtual bool onKeyUp(KeyCode key)
	{
		LOG_DEBUG("[Msgboxwnd] Window::onKeyUp:" << key);

		if (key == KEY_RIGHT)
		{
			setFocusChild(&m_cancel);
			return true;
		}
		else if (key == KEY_LEFT)
		{
			setFocusChild(&m_ok);
			return true;
		}
		else if (key == KEY_CLOSE || key == KEY_BACK)
		{
			WindowManager::getManager()->setFocus(m_active);
			// send
			onSelect(MSGWNDBTN_CLOSE);
			delete this;
			return true;
		}

		return false;
	}

private:
	TextWnd m_title;
	TextWnd m_msg;
	ButtonWnd m_ok;
	ButtonWnd m_cancel;
	Window* m_active;
	Window* m_listener;
};

Window* ShowMessageWindow(Window* listener, std::string title, std::string msg, MessageWindowButtonStyle style,  std::string strOK, std::string strCancel)
{
	return new MessageWnd(listener, title, msg, style, strOK, strCancel);
}

