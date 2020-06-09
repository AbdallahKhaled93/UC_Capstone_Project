#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <memory>
#include <mutex>
#include <future>
#include <wx/wx.h>

#include "Node.h"

// middle part of the window containing the dialog between user and chatbot
class ChatBotPanelDialog : public wxScrolledWindow
{
private:
    // control elements
    wxBoxSizer *_dialogSizer;
    wxBitmap _image;


public:
    // constructor / destructor
    ChatBotPanelDialog(wxWindow *parent, wxWindowID id);
    ~ChatBotPanelDialog();

    // getter / setter

    // events
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);

    // proprietary functions
    void AddDialogItem(wxString text, bool isFromUser = true);

    DECLARE_EVENT_TABLE()
};

// dialog item shown in ChatBotPanelDialog
class ChatBotPanelDialogItem : public wxPanel
{
private:
    // control elements
    wxStaticText *_chatBotTxt;

public:
    // constructor / destructor
    ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser);
};

// Forward class declaration of the thread class
class ReceptionThread;
// frame containing all control elements
class ChatBotFrame : public wxFrame
{
private:
    // control elements
    ChatBotPanelDialog *_panelDialog;
    wxTextCtrl *_userTextCtrl;
    // chat node
    std::unique_ptr<Node> _chatNode;
    // received message
    std::string receivedMessage;
    // reception thread that consumes from message queue
    ReceptionThread*_receptionThread;
    // events
    void OnEnter(wxCommandEvent &WXUNUSED(event));
    void OnDisplayCommand(wxThreadEvent &WXUNUSED(event));
    void OnClose(wxCloseEvent &WXUNUSED(event));

public:
    // constructor / desctructor
    ChatBotFrame(const wxString &title, std::unique_ptr<Node> &node_ptr);
    ~ChatBotFrame();

    friend class ReceptionThread;
};

// control panel for background image display
class ChatBotFrameImagePanel : public wxPanel
{
    // control elements
    wxBitmap _image;

public:
    // constructor / desctructor
    ChatBotFrameImagePanel(wxFrame *parent);

    // events
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);

    DECLARE_EVENT_TABLE()
};

// wxWidgets app that hides main()
class ChatBotApp : public wxApp
{
public:
    // events
    virtual bool OnInit();

private:
    std::unique_ptr<Node> _chatNode;
};

class ReceptionThread : public wxThread
{
public:
    ~ReceptionThread();
    void setFrame(ChatBotFrame *frame);
protected:
    virtual ExitCode Entry();
    ChatBotFrame *frame;
};

#endif /* CHATGUI_H_ */
