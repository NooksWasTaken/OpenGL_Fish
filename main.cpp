#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/timer.h>
#include <windows.h>
#include <GL/gl.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

// OpenGL Canvas
class MyGLCanvas : public wxGLCanvas {
public:
    MyGLCanvas(wxWindow* parent)
        : wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
        m_gaugeValue(10), m_points(0), m_animationActive(false),
        m_fishColorR(1.0f), m_fishColorG(0.75f), m_fishColorB(0.30f) { // Default fish color
        m_context = new wxGLContext(this);
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        // Initialize timer
        m_timer = new wxTimer(this);
        Bind(wxEVT_TIMER, &MyGLCanvas::OnTimer, this, m_timer->GetId());

        // Bind events
        Bind(wxEVT_PAINT, &MyGLCanvas::OnPaint, this);
        Bind(wxEVT_SIZE, &MyGLCanvas::OnResize, this);

        srand(static_cast<unsigned>(time(nullptr)));

    }

    ~MyGLCanvas() {
        delete m_context;
        delete m_timer;
    }

    void DecreaseGauge() {
        if (m_gaugeValue > 0) {
            --m_gaugeValue;
        }
        if (m_gaugeValue == 0) {
            m_points += 200;      // Add points when the gauge reaches 0
            m_gaugeValue = m_gaugeMax + 1; // Increment health and reset value
            m_gaugeMax += 1;      // Scale health bar by 2
            StartAnimation();     // Trigger animation

            // Randomize the fish body color
            m_fishColorR = static_cast<float>(rand()) / RAND_MAX;
            m_fishColorG = static_cast<float>(rand()) / RAND_MAX;
            m_fishColorB = static_cast<float>(rand()) / RAND_MAX;
        }
        Refresh(); // Redraw the canvas
    }

    int GetPoints() const { return m_points; }
    int GetGaugeMax() const { return m_gaugeMax; }
    int GetGaugeValue() const { return m_gaugeValue; }

private:
    void StartAnimation() {
        m_animationActive = true;
        m_circlePositions.clear();

        // Generate random x and y coordinates for circles
        for (int i = 0; i < 30; ++i) {
            float x = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f)); // Between -1.0 and 1.0
            float y = 0.8f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.2f)); // Varies the height
            m_circlePositions.push_back({ x, y }); // Start at random height
        }

        m_timer->Start(30); // Update every 30ms
    }

    void OnTimer(wxTimerEvent& event) {
        if (m_animationActive) {
            for (auto& pos : m_circlePositions) {
                pos.second -= 0.05f; // Move downward
            }

            // Stop animation if all circles are out of bounds
            if (std::all_of(m_circlePositions.begin(), m_circlePositions.end(), [](const auto& pos) {
                return pos.second < -1.0f;
                })) {
                m_animationActive = false;
                m_timer->Stop();
            }

            Refresh(); // Redraw canvas
        }
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        SetCurrent(*m_context);

        // Clear the OpenGL canvas
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // Get the aspect ratio
        int width, height;
        GetClientSize(&width, &height);
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        // Background
        glColor3f(0.53f, 0.81f, 0.90f);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glEnd();

        // Fish
        glColor3f(m_fishColorR, m_fishColorG, m_fishColorB);
        glBegin(GL_POLYGON);
        glVertex2f(-0.5f, 0.25f);  // Top-left
        glVertex2f(-0.25f, 0.45f); // Curve top-left
        glVertex2f(0.25f, 0.45f);  // Curve top-right
        glVertex2f(0.5f, 0.25f);   // Top-right
        glVertex2f(0.5f, -0.25f);  // Bottom-right
        glVertex2f(0.25f, -0.45f); // Curve bottom-right
        glVertex2f(-0.25f, -0.45f); // Curve bottom-left
        glVertex2f(-0.5f, -0.25f);  // Bottom-left
        glEnd();

        glColor3f(m_fishColorR, m_fishColorG, m_fishColorB);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.3f, 0.0f);
        glVertex2f(-0.7f, 0.45f);
        glVertex2f(-0.7f, -0.45f);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2f(0.5f, 0.25f);
        glVertex2f(0.5f, -0.25f);
        glVertex2f(0.65f, 0.0f);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.2f, 0.4f);
        glVertex2f(0.0f, 0.65f);
        glVertex2f(0.2f, 0.4f);
        glEnd();

        // Eye
        glColor3f(0.0f, 0.0f, 0.0f);
        glPointSize(8.0f);
        glBegin(GL_POINTS);
        glVertex2f(0.5f, 0.1f);
        glEnd();

        // Falling hollow circles
        if (m_animationActive) {
            glColor3f(0.88f, 1.0f, 1.0f); // Light cyan (hollow circles)
            for (const auto& pos : m_circlePositions) {
                float x = pos.first;
                float y = pos.second;

                glBegin(GL_LINE_LOOP); // Draw a hollow circle
                for (int i = 0; i <= 20; ++i) {
                    float angle = i * 2.0f * M_PI / 20; // 20 segments
                    glVertex2f(x + 0.05f * cos(angle) / aspectRatio, y + 0.05f * sin(angle));
                }
                glEnd();
            }
        }

        // Health bar background (red)
        glColor3f(1.0f, 0.0f, 0.0f); // Red background
        glBegin(GL_QUADS);
        glVertex2f(-0.8f, -0.6f);  // Top-left
        glVertex2f(0.8f, -0.6f);   // Top-right
        glVertex2f(0.8f, -0.7f);   // Bottom-right
        glVertex2f(-0.8f, -0.7f);  // Bottom-left
        glEnd();

        // Calculate the green bar's width
        float maxBarWidth = 1.6f; // Full width of the health bar
        float gaugeWidth = (m_gaugeValue / static_cast<float>(m_gaugeMax)) * maxBarWidth;

        // Ensure full alignment when gauge is at maximum
        if (m_gaugeValue == m_gaugeMax) {
            gaugeWidth = maxBarWidth;
        }

        // Draw the green health bar
        glColor3f(0.0f, 1.0f, 0.0f); // Green (current health)
        glBegin(GL_QUADS);
        glVertex2f(-0.8f, -0.6f); // Top-left
        glVertex2f(-0.8f + gaugeWidth, -0.6f); // Top-right
        glVertex2f(-0.8f + gaugeWidth, -0.7f); // Bottom-right
        glVertex2f(-0.8f, -0.7f); // Bottom-left
        glEnd();

        // Border for health bar
        glColor3f(1.0f, 0.83f, 0.28f); // Gold border
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
    int m_gaugeMax = 10;
    int m_points;
    bool m_animationActive;
    std::vector<std::pair<float, float>> m_circlePositions;
    wxTimer* m_timer;

    // Fish body color
    float m_fishColorR;
    float m_fishColorG;
    float m_fishColorB;
};

class MyFrame : public wxFrame {
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "FISH TANK'D", wxDefaultPosition, wxSize(800, 600)) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        m_canvas = new MyGLCanvas(this);
        sizer->Add(m_canvas, 1, wxEXPAND);

        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* button = new wxButton(this, wxID_ANY, "SMACK");
        button->Bind(wxEVT_BUTTON, &MyFrame::OnButtonClicked, this);

        m_pointsText = new wxStaticText(this, wxID_ANY, "Points Earned: 0", wxDefaultPosition, wxSize(150, 30), wxALIGN_LEFT);
        m_gaugeText = new wxStaticText(this, wxID_ANY, "Health: 10", wxDefaultPosition, wxSize(150, 30), wxALIGN_LEFT);

        buttonSizer->Add(m_gaugeText, 0, wxALIGN_LEFT | wxRIGHT, 10);
        buttonSizer->Add(button, 0, wxALIGN_CENTER);
        buttonSizer->Add(m_pointsText, 0, wxALIGN_RIGHT | wxLEFT, 10);
        sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

        SetSizer(sizer);
        StartBackgroundMusic();

    }

private:
    void StartBackgroundMusic() {
        // Play the sound on repeat
        PlaySound(TEXT("C:\\Users\\Michael Jerome Reyes\\source\\repos\\OpenGL_Game\\OpenGL_Game\\FISH_BGM.wav"), NULL, SND_ASYNC | SND_LOOP);
    }

    void OnButtonClicked(wxCommandEvent& event) {
        m_canvas->DecreaseGauge();
        UpdateUI();
    }

    void UpdateUI() {
        m_pointsText->SetLabelText(wxString::Format("Points Earned: %d", m_canvas->GetPoints()));
        m_gaugeText->SetLabelText(wxString::Format("Health: %d", m_canvas->GetGaugeValue()));
    }

    MyGLCanvas* m_canvas;
    wxStaticText* m_pointsText;
    wxStaticText* m_gaugeText;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
