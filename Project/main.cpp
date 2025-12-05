#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/textdlg.h> 
#include "chat_logic.hpp"

enum {
    ID_BTN_SEND = 1001,
    ID_BTN_CONNECT = 1002
};

class ChatFrame : public wxFrame {
public:
    ChatFrame(const wxString& nickname) 
        : wxFrame(nullptr, wxID_ANY, "C++ P2P Chat", wxDefaultPosition, wxSize(500, 400)), 
          m_nickname(nickname)
    {
        SetTitle(wxString::Format("P2P Chat - Користувач: %s", m_nickname));

        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* connSizer = new wxBoxSizer(wxHORIZONTAL);
        
        txtMyPort = new wxTextCtrl(panel, wxID_ANY, "8080", wxDefaultPosition, wxSize(50, -1));
        btnStartServer = new wxButton(panel, wxID_ANY, "Start Server");
        
        txtTargetIP = new wxTextCtrl(panel, wxID_ANY, "127.0.0.1");
        txtTargetPort = new wxTextCtrl(panel, wxID_ANY, "8081", wxDefaultPosition, wxSize(50, -1));
        btnConnect = new wxButton(panel, ID_BTN_CONNECT, "Connect");

        connSizer->Add(new wxStaticText(panel, wxID_ANY, "My Port:"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        connSizer->Add(txtMyPort, 0, wxALL, 5);
        connSizer->Add(btnStartServer, 0, wxALL, 5);
        connSizer->Add(new wxStaticLine(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxEXPAND | wxALL, 5);
        connSizer->Add(new wxStaticText(panel, wxID_ANY, "Peer IP:"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        connSizer->Add(txtTargetIP, 1, wxALL, 5);
        connSizer->Add(txtTargetPort, 0, wxALL, 5);
        connSizer->Add(btnConnect, 0, wxALL, 5);

        txtChatHistory = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

        wxString instruction = 
       "---P2P Chat Instructions---\n"
       "1. **Start Server:** Enter your port (e.g., 8080) and click 'Start Server'. You will be waiting for a connection.\n"
       "2. **Client Connection:** The other participant must enter your IP (Peer IP) and your port (Peer Port, e.g., 8080).\n"
       "3. **Connection:** The client clicks 'Connect'. After a successful connection, a message 'Connected to peer!' will appear.\n"
       "4. **Local Test:** For testing on a single PC, use the IP '127.0.0.1'.\n";

    txtChatHistory->AppendText(instruction);

        wxBoxSizer* msgSizer = new wxBoxSizer(wxHORIZONTAL);
        
        txtMessage = new wxTextCtrl(panel, wxID_ANY, wxString::FromUTF8("Привіт!"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        btnSend = new wxButton(panel, ID_BTN_SEND, "Send");

        msgSizer->Add(txtMessage, 1, wxALL, 5);
        msgSizer->Add(btnSend, 0, wxALL, 5);

        mainSizer->Add(connSizer, 0, wxEXPAND | wxALL, 5);
        mainSizer->Add(txtChatHistory, 1, wxEXPAND | wxALL, 5);
        mainSizer->Add(msgSizer, 0, wxEXPAND | wxALL, 5);
        panel->SetSizer(mainSizer);

        network = std::make_unique<ChatNetwork>([this](const std::string& msg) {
            wxTheApp->CallAfter([this, msg]() {
                txtChatHistory->AppendText(wxString::FromUTF8(msg) + "\n");
            });
        });

        btnStartServer->Bind(wxEVT_BUTTON, &ChatFrame::OnStartServer, this);
        btnConnect->Bind(wxEVT_BUTTON, &ChatFrame::OnConnect, this);
        btnSend->Bind(wxEVT_BUTTON, &ChatFrame::OnSend, this);
        txtMessage->Bind(wxEVT_TEXT_ENTER, &ChatFrame::OnSend, this);
    }

private:
    void OnStartServer(wxCommandEvent& event) {
        long port;
        if (txtMyPort->GetValue().ToLong(&port)) {
            network->startServer((unsigned short)port);
            btnStartServer->Disable();
        }
    }

    void OnConnect(wxCommandEvent& event) {
        std::string ip = std::string(txtTargetIP->GetValue().ToUTF8());
        std::string port = std::string(txtTargetPort->GetValue().ToUTF8());
        network->connectToPeer(ip, port);
    }

    void OnSend(wxCommandEvent& event) {
        wxString wxMsg = txtMessage->GetValue();
        if (!wxMsg.empty()) {
            std::string msgUtf8 = std::string(wxMsg.ToUTF8());
            std::string nickUtf8 = std::string(m_nickname.ToUTF8());

            network->sendMessage(nickUtf8, msgUtf8);
            
            txtChatHistory->AppendText(m_nickname + ": " + wxMsg + "\n");
            txtMessage->Clear();
        }
    }

    wxTextCtrl* txtMyPort;
    wxButton* btnStartServer;
    wxTextCtrl* txtTargetIP;
    wxTextCtrl* txtTargetPort;
    wxButton* btnConnect;
    wxTextCtrl* txtChatHistory;
    wxTextCtrl* txtMessage;
    wxButton* btnSend;

    wxString m_nickname;
    std::unique_ptr<ChatNetwork> network;
};

class ChatApp : public wxApp {
public:
    virtual bool OnInit() {
        wxInitAllImageHandlers();

        wxString defaultNick = wxString::FromUTF8("Гість");
        wxString caption = wxString::FromUTF8("Вхід");
        wxString message = wxString::FromUTF8("Введіть ваш нікнейм:");

        wxTextEntryDialog dialog(nullptr, message, caption, defaultNick, wxOK | wxCANCEL | wxCENTRE);

        if (dialog.ShowModal() == wxID_OK) {
            wxString nickname = dialog.GetValue();
            if (nickname.IsEmpty()) nickname = defaultNick;

            ChatFrame* frame = new ChatFrame(nickname);
            frame->Show(true);
            return true;
        }

        return false;
    }
};

wxIMPLEMENT_APP(ChatApp);