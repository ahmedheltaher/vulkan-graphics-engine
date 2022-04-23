#include "./first_app.hpp"

namespace App {
	void FirstApp::Run() {
		while (!m_Window.IsClosed()) {
			m_Window.Update();
		}
	}
}