#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <memory>
#include <mutex>
#include <future>
#include <wx/wx.h>

#include "Node.h"
#include "MessageQueue.h"

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
    void PrintChatbotResponse(std::string response);

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

// frame containing all control elements
class ChatBotFrame : public wxFrame
{
private:
    // control elements
    ChatBotPanelDialog *_panelDialog;
    wxTextCtrl *_userTextCtrl;
    std::unique_ptr<Node> _chatNode;
    std::shared_ptr<MessageQueue> _toBeDisplayedMessages;
    // mutex to protect the dialog
    std::mutex _mtxDialogItems;
    std::string toBeDisplayedNext;

    // events
    void OnEnter(wxCommandEvent &WXUNUSED(event));
    void OnUpdate(wxDisplayChangedEvent &WXUNUSED(event));

public:
    // constructor / desctructor
    ChatBotFrame(const wxString &title);
    // get node pointer
    void passNodeObject(std::unique_ptr<Node> &node_ptr);
    // function to get shared message queue with node object
    void getMessageQueue();

    friend void DisplayPollingThreadFunction(ChatBotFrame &frame);
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

#endif /* CHATGUI_H_ */
