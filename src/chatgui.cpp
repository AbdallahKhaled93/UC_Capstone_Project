#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/image.h>
#include <string>
#include <mutex>
#include "chatgui.h"
#include "Server.h"
#include "Client.h"

// size of chatbot window
const int width = 414;
const int height = 736;

#define PORT 8080

// wxWidgets APP
IMPLEMENT_APP(ChatBotApp);

std::string dataPath = "../";
std::string imgBasePath = dataPath + "images/";

void usage()
{
    std::cout << "Incorrect arguments" << std::endl;
    std::cout << "Usage: ./ChatApp OPTION" << std::endl;
    std::cout << "c : client" << std::endl;
    std::cout << "s : server" << std::endl;
}

bool ChatBotApp::OnInit()
{
    if(wxApp::argc != 2)
    {
        usage();
        return 1;
    }

    std::string Ldirection = std::string(wxApp::argv[1]);

    if((Ldirection != "c") && (Ldirection != "s"))
    {
        usage();
        return 1;
    }

    /* code if server */
    if(Ldirection == "s")
    {
        try
        {
            Node* nodeRawPtr = new Server("127.0.0.1",PORT);
            _chatNode = std::unique_ptr<Node>(nodeRawPtr);
            ((Server*)(_chatNode.get()))->acceptClientConnections();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }
    }

    if(Ldirection == "c")
    {
        try
        {
            Node* nodeRawPtr = new Client();
            _chatNode = std::unique_ptr<Node>(nodeRawPtr);
            ((Client*)(_chatNode.get()))->connectToServer("127.0.0.1",PORT);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }
    }

    // create window with name and show it
    ChatBotFrame *chatBotFrame = new ChatBotFrame(wxT(direction), _chatNode);
    chatBotFrame->Show(true);

    return true;
}

// wxWidgets FRAME
// This event will be used to notify the main thread to add a dialogue item
wxDECLARE_EVENT(wxEVT_COMMAND_DISPLAY_MESSAGE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_DISPLAY_MESSAGE, wxThreadEvent);
ChatBotFrame::ChatBotFrame(const wxString &title, std::unique_ptr<Node> &node_ptr) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(width, height))
{
    // create panel with background image
    ChatBotFrameImagePanel *ctrlPanel = new ChatBotFrameImagePanel(this);

    // create controls and assign them to control panel
    _panelDialog = new ChatBotPanelDialog(ctrlPanel, wxID_ANY);

    // get chat node object from wx app
    _chatNode = std::move(node_ptr);

    // create text control for user input
    int idTextXtrl = 1;
    _userTextCtrl = new wxTextCtrl(ctrlPanel, idTextXtrl, "", wxDefaultPosition, wxSize(width, 50), wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);
    Connect(idTextXtrl, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ChatBotFrame::OnClose));
    this->Connect(wxEVT_COMMAND_DISPLAY_MESSAGE, wxThreadEventHandler(ChatBotFrame::OnDisplayCommand));

    // create vertical sizer for panel alignment and add panels
    wxBoxSizer *vertBoxSizer = new wxBoxSizer(wxVERTICAL);
    vertBoxSizer->AddSpacer(90);
    vertBoxSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
    vertBoxSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
    ctrlPanel->SetSizer(vertBoxSizer);

    // position window in screen center
    this->Centre();

    // Start reception thread
    _receptionThread = new ReceptionThread();
    _receptionThread->setFrame(this);
    if ( _receptionThread->Run() != wxTHREAD_NO_ERROR )
    {
        std::cout << "Can't create the thread!" << std::endl;
        delete _receptionThread;
        _receptionThread = NULL;
    }
}

ChatBotFrame::~ChatBotFrame()
{
}

void ChatBotFrame::OnEnter(wxCommandEvent &WXUNUSED(event))
{
    // retrieve text from text control
    std::string userText = std::string(_userTextCtrl->GetLineText(0));

    // send message
    _chatNode->sendMessage(userText);

    // add new user text to dialog
    _panelDialog->AddDialogItem(userText, true);

    // delete text in text control
    _userTextCtrl->Clear();
}

void ChatBotFrame::OnDisplayCommand(wxThreadEvent &WXUNUSED(event))
{
    std::lock_guard _gLock(_mtxForString);
    this->_panelDialog->AddDialogItem(this->receivedMessage, false);
}

void ChatBotFrame::OnClose(wxCloseEvent &WXUNUSED(event))
{
    this->_receptionThread->Delete();
    Destroy();
}

BEGIN_EVENT_TABLE(ChatBotFrameImagePanel, wxPanel)
EVT_PAINT(ChatBotFrameImagePanel::paintEvent) // catch paint events
END_EVENT_TABLE()

ChatBotFrameImagePanel::ChatBotFrameImagePanel(wxFrame *parent) : wxPanel(parent)
{
}

void ChatBotFrameImagePanel::paintEvent(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void ChatBotFrameImagePanel::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void ChatBotFrameImagePanel::render(wxDC &dc)
{
    // load backgroud image from file
    wxString imgFile = imgBasePath + "background.jpg";
    wxImage image;
    image.LoadFile(imgFile);

    // rescale image to fit window dimensions
    wxSize sz = this->GetSize();
    wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
    _image = wxBitmap(imgSmall);

    dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent) // catch paint events
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow *parent, wxWindowID id)
    : wxScrolledWindow(parent, id)
{
    // sizer will take care of determining the needed scroll size
    _dialogSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_dialogSizer);

    // allow for PNG images to be handled
    wxInitAllImageHandlers();

}

ChatBotPanelDialog::~ChatBotPanelDialog()
{
}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser)
{
    // add a single dialog element to the sizer
    ChatBotPanelDialogItem *item = new ChatBotPanelDialogItem(this, text, isFromUser);
    _dialogSizer->Add(item, 0, wxALL | (isFromUser == true ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
    _dialogSizer->Layout();

    // make scrollbar show up
    this->FitInside(); // ask the sizer about the needed size
    this->SetScrollRate(5, 5);
    this->Layout();

    // scroll to bottom to show newest element
    int dx, dy;
    this->GetScrollPixelsPerUnit(&dx, &dy);
    int sy = dy * this->GetScrollLines(wxVERTICAL);
    this->DoScroll(0, sy);
}

void ChatBotPanelDialog::paintEvent(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void ChatBotPanelDialog::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void ChatBotPanelDialog::render(wxDC &dc)
{
    wxImage image;
    image.LoadFile(imgBasePath + "background.jpg");

    wxSize sz = this->GetSize();
    wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);

    _image = wxBitmap(imgSmall);
    dc.DrawBitmap(_image, 0, 0, false);
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser)
    : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE)
{

    // create text
    _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1), wxALIGN_CENTRE | wxBORDER_NONE);
    _chatBotTxt->SetForegroundColour(isFromUser == true ? wxColor(*wxBLACK) : wxColor(*wxWHITE));

    // create sizer and add elements
    wxBoxSizer *horzBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    horzBoxSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
    this->SetSizer(horzBoxSizer);

    // wrap text after 150 pixels
    _chatBotTxt->Wrap(150);

    // set background color
    this->SetBackgroundColour((isFromUser == true ? wxT("YELLOW") : wxT("BLUE")));
}


// Reception thread functions definition
wxThread::ExitCode ReceptionThread::Entry()
{
    while (!TestDestroy())
    {
        std::string temp;
        /* use dynamic cast to call the appropriate polling fuction */
        Server*s = dynamic_cast<Server*>(this->frame->_chatNode.get());

        /* this is client node */
        if(s == 0)
        {
            Client*c = dynamic_cast<Client*>(this->frame->_chatNode.get());
            temp = c->receiveMessage();
        }
        // server node
        else
        {
            temp = s->receiveMessage();
        }


        if(temp != "")
        {
            frame->_mtxForString.lock();
            frame->receivedMessage = temp;
            frame->_mtxForString.unlock();
            wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_DISPLAY_MESSAGE));
        }
    }

    std::cout << "Thread ended" << std::endl;

    return (wxThread::ExitCode)0;
}

void ReceptionThread::setFrame(ChatBotFrame *frame)
{
    this->frame = frame;
}

ReceptionThread::~ReceptionThread()
{
    std::cout << "Thread destroyed" << std::endl;
    // the thread is being destroyed; make sure not to leave dangling pointers around
    frame->_receptionThread = nullptr;
}
