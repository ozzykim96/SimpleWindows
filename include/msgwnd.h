#ifndef _MSGWND_H_
#define _MSGWND_H_

enum MessageWindowButtonStyle
{
	MSGWNDBTNSTYLE_OK,
	MSGWNDBTNSTYLE_OKCANCEL
};

enum MessageWindowButton
{
	MSGWNDBTN_OK,
	MSGWNDBTN_CANCEL,
	MSGWNDBTN_CLOSE
};

/**
 * show message window. when selecting the button, it will generate WE_SELECT with MessageWindowButton.
 * @note do not destroy the returned window. it destroys itself.
 */
Window* ShowMessageWindow(Window* listener, std::string title, std::string msg, MessageWindowButtonStyle style, std::string strOK="", std::string strCancel="");

#endif /* _MSGWND_H_ */
