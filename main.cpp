#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <windows.h>
#include <GL/gl.h>

class MyGLCanvas : public wxGLCanvas {
public:
    MyGLCanvas(wxWindow* parent)
        : wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
        m_gaugeValue(10), m_points(0) {
        m_context = new wxGLContext(this);
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        // Bind events
        Bind(wxEVT_PAINT, &MyGLCanvas::OnPaint, this);
        Bind(wxEVT_SIZE, &MyGLCanvas::OnResize, this);
    }

    ~MyGLCanvas() {
        delete m_context;
    }

    void DecreaseGauge() {
        if (m_gaugeValue > 0) {
            --m_gaugeValue;
        }
        if (m_gaugeValue == 0) {
            m_points += 200; // Add points when the gauge reaches 0
            m_gaugeValue = 10; // Resets bar
        }
        Refresh(); // Redraw the canvas
    }

    int GetPoints() const { return m_points; }
    int GetGaugeValue() const { return m_gaugeValue; }

private:
    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        SetCurrent(*m_context);

        // Clear the OpenGL canvas
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // Background
        glColor3f(0.53f, 0.81f, 0.90f);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glEnd();

        // Fish body
        glColor3f(1.0f, 0.75f, 0.30f);
        glBegin(GL_POLYGON);
        // Front part
        glVertex2f(-0.5f, 0.25f);  // Top-left
        glVertex2f(-0.25f, 0.45f); // Curve top-left
        glVertex2f(0.25f, 0.45f);  // Curve top-right
        glVertex2f(0.5f, 0.25f);   // Top-right
        // Back part
        glVertex2f(0.5f, -0.25f);  // Bottom-right
        glVertex2f(0.25f, -0.45f); // Curve bottom-right
        glVertex2f(-0.25f, -0.45f); // Curve bottom-left
        glVertex2f(-0.5f, -0.25f);  // Bottom-left
        glEnd();

        // Fish tail
        glColor3f(1.0f, 0.75f, 0.30f);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.3f, 0.0f);  // Left tip of the tail
        glVertex2f(-0.70f, 0.45f); // Tail top left
        glVertex2f(-0.70f, -0.45f); // Tail bottom left
        glEnd();

        // Fish head
        glColor3f(1.0f, 0.75f, 0.30f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.5f, 0.25f);   // Top corner
        glVertex2f(0.5f, -0.25f);  // Bottom corner
        glVertex2f(0.65f, 0.0f);    // Tip
        glEnd();

        // Fish fin on top
        glColor3f(1.0f, 0.75f, 0.30f);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.2f, 0.4f);  // Base of the fish
        glVertex2f(-0.0f, 0.65f); // Tip of the fin
        glVertex2f(0.2f, 0.4f);   // Base of the fin
        glEnd();

        // Eye on the fish
        glColor3f(0.0f, 0.0f, 0.0f); // Black eye
        glPointSize(8.0f);
        glBegin(GL_POINTS);
        glVertex2f(0.5f, 0.1f);  // Eye position
        glEnd();

        // Health bar background (red)
        glColor3f(1.0f, 0.0f, 0.0f); // Red background
        glBegin(GL_QUADS);
        glVertex2f(-0.8f, -0.6f);  // Top-left
        glVertex2f(0.8f, -0.6f);   // Top-right
        glVertex2f(0.8f, -0.7f);   // Bottom-right
        glVertex2f(-0.8f, -0.7f);  // Bottom-left
        glEnd();

        // Health bar
        float gaugeWidth = (1.6f * m_gaugeValue) / 10.0f;
        glColor3f(0.0f, 1.0f, 0.0f); // Green bar
        glBegin(GL_QUADS);
        glVertex2f(-0.8f, -0.6f);
        glVertex2f(-0.8f + gaugeWidth, -0.6f);
        glVertex2f(-0.8f + gaugeWidth, -0.7f);
        glVertex2f(-0.8f, -0.7f);
        glEnd();

        // Border for health bar
        glColor3f(1.0f, 0.83f, 0.28f);
        glLineWidth(2.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.8f, -0.6f);
        glVertex2f(0.8f, -0.6f);
        glVertex2f(0.8f, -0.7f);
        glVertex2f(-0.8f, -0.7f);
        glEnd();

        SwapBuffers();
    }

    void OnResize(wxSizeEvent& event) {
        int width, height;
        GetClientSize(&width, &height);
        glViewport(0, 0, width, height);
        Refresh();
    }

    wxGLContext* m_context;
    int m_gaugeValue;
    int m_points;
};

class MyFrame : public wxFrame {
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "FISH TANK'D", wxDefaultPosition, wxSize(800, 600)) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        m_canvas = new MyGLCanvas(this);
        sizer->Add(m_canvas, 1, wxEXPAND);

        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* button = new wxButton(this, wxID_ANY, "FISH");
        button->Bind(wxEVT_BUTTON, &MyFrame::OnButtonClicked, this);

        m_pointsText = new wxStaticText(this, wxID_ANY, "Points Earned: 0", wxDefaultPosition, wxSize(150, 30), wxALIGN_LEFT);
        m_gaugeText = new wxStaticText(this, wxID_ANY, "Health: 10", wxDefaultPosition, wxSize(150, 30), wxALIGN_LEFT);

        buttonSizer->Add(m_gaugeText, 0, wxALIGN_LEFT | wxRIGHT, 10);
        buttonSizer->Add(button, 0, wxALIGN_CENTER);
        buttonSizer->Add(m_pointsText, 0, wxALIGN_RIGHT | wxLEFT, 10);
        sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

        SetSizer(sizer);
    }

private:
    void OnButtonClicked(wxCommandEvent& event) {
        m_canvas->DecreaseGauge();
        UpdateUI();
        PlaySoundOverlay();
    }

    void UpdateUI() {
        m_pointsText->SetLabel("Points Earned: " + std::to_string(m_canvas->GetPoints()));
        m_gaugeText->SetLabel("Health: " + std::to_string(m_canvas->GetGaugeValue()));
    }

    void PlaySoundOverlay() {
        PlaySound(TEXT("C:\\Users\\Michael Jerome Reyes\\source\\repos\\OpenGL_Game\\OpenGL_Game\\FISH.wav"), NULL, SND_ASYNC | SND_NOSTOP);
    }

    MyGLCanvas* m_canvas;
    wxStaticText* m_pointsText;
    wxStaticText* m_gaugeText;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
