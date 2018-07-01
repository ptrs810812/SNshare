#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_internal.h"
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <vector>
#include <sstream>

#include <stdlib.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>

#include <iostream>

#define WINDOW_NAME u8"瑪奇序號分享"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 300
#define IMGUI_WINDOW_SIZE ImVec2(800, 300)


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

struct search_list
{
	std::string event_name;
	std::vector<std::string> item_name;
	void clear()
	{
		event_name.clear();
		item_name.clear();
	}
};
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
//int main()
{
	sql::Driver *driver;
	static sql::Connection *con;
	static sql::Statement *stmt;
	sql::ResultSet *res;
	static std::vector<search_list> list;
	static int event_inuse = 0, item_inuse = 0;
	std::string serial_number,last_serial_number="";
	static int primary_key;
	try {
		// create link to mysql
		driver = get_driver_instance();
		con = driver->connect("122.116.232.94:3306", "mabi_sn_share_ptrs", "mabi_sn_share_pass");
		con->setSchema("mabi");
		stmt = con->createStatement();
		res = stmt->executeQuery(u8"SELECT distinct event_name FROM sn_share");
		while (res->next())
		{
			search_list temp;
			temp.event_name = res->getString("event_name").c_str();				//initialize all evnet_names
			list.push_back(temp);
		}

		std::stringstream ss;
		for (int i = 0; i < list.size(); i++)
		{
			ss.str("");
			ss << u8"SELECT distinct item_name FROM sn_share where event_name = '" << list[i].event_name.c_str() << "' order by item_name;";
			res = stmt->executeQuery(ss.str().c_str());
			while (res->next())
			{
				std::string temp;
				temp = res->getString("item_name").c_str();
				list[i].item_name.push_back(temp);
			}
		}

		delete res;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR:SQLException in" << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << "\n";
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << "\n";
		if (e.getErrorCode() == 1062)	std::cout << "same sn \n";
	}


	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Freeconsle
	//FreeConsole();

	// Create window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();


	// Load image
	int w, h, nrChannels;
	unsigned char *data = stbi_load("./pic/refresh.png", &w, &h, &nrChannels, 0);
	unsigned int texture_login;
	glGenTextures(1, &texture_login);
	glBindTexture(GL_TEXTURE_2D, texture_login);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	data = stbi_load("./pic/get.png", &w, &h, &nrChannels, 0);
	unsigned int texture_get;
	glGenTextures(1, &texture_get);
	glBindTexture(GL_TEXTURE_2D, texture_get);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	data = stbi_load("./pic/share.png", &w, &h, &nrChannels, 0);
	unsigned int texture_share;
	glGenTextures(1, &texture_share);
	glBindTexture(GL_TEXTURE_2D, texture_share);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);





	//can use it to set window position when window not decorated
	//glfwSetWindowPos(window, 100, 100);//need to modify to fit imgui moving
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//enable show chinese
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\mingliu.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());//微軟正黑
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msjh.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());//微軟正黑

																													 //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
																													 //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsDark();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 0.0f);

	ImVec4 clear_color = ImVec4(0.25f, 0.53f, 0.68f, 1.00f);

	bool show_main_window = true;
	bool show_type_window = true;
	bool show_login_child = false;
	bool show_copy_child = true;
	bool show_share_child = false;
	bool show_share_text = true;

	bool show_get_button = false;
	bool show_serial_number = false;
	static const char* type_sel = list[0].event_name.c_str();

	// Main loop
	while (!glfwWindowShouldClose(window) & show_main_window)
	{
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();


		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
		if (show_main_window) {
			ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoTitleBar;
			ImGuiWindowFlags share_flag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
			ImGui::Begin("window", &show_main_window, main_flags);
			ImGui::SetWindowSize(IMGUI_WINDOW_SIZE, true);
			ImGui::SetWindowPos(ImVec2(0, 0), true);

			ImGuiColumnsFlags main_cloumn_flags = ImGuiColumnsFlags_NoResize;
			ImGui::Columns(2, "main_column", true, main_cloumn_flags);  // 2-ways, with border
			ImGui::SetColumnWidth(0, 60);	//this column for: login, get sn, share sn

											// Image button
			{
				ImVec2 buttonsize = { 40.0f,40.0f };
				if (ImGui::ImageButton((void*)texture_login, buttonsize, ImVec2(0, 0), ImVec2(1, 1)))
				{
					show_login_child = false; show_copy_child = true; show_share_child = false;

					list.clear();
					res = stmt->executeQuery(u8"SELECT distinct event_name FROM sn_share");
					while (res->next())
					{
						search_list temp;
						temp.event_name = res->getString("event_name").c_str();				//initialize all evnet_names
						list.push_back(temp);
					}

					std::stringstream ss;
					for (int i = 0; i < list.size(); i++)
					{
						ss.str("");
						ss << u8"SELECT distinct item_name FROM sn_share where event_name = '" << list[i].event_name.c_str() << "' order by item_name;";
						res = stmt->executeQuery(ss.str().c_str());
						while (res->next())
						{
							std::string temp;
							temp = res->getString("item_name").c_str();
							list[i].item_name.push_back(temp);
						}
					}
					type_sel = list[0].event_name.c_str();
					delete res;
				}
				if (ImGui::ImageButton((void*)texture_get, buttonsize, ImVec2(0, 0), ImVec2(1, 1)))
				{
					show_login_child = false; show_copy_child = true; show_share_child = false;
				}
				if (ImGui::ImageButton((void*)texture_share, buttonsize, ImVec2(0, 0), ImVec2(1, 1)))
				{
					show_login_child = false; show_copy_child = false; show_share_child = true;
				}
			}

			ImGui::NextColumn();
			if (show_login_child == true)											//leave for login function, now is refresh list
			{
				/*ImGui::BeginChild("login", ImVec2(0, 0), true, main_flags);
				ImGui::Text("this is login\n");
				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				std::stringstream ss;
				for (int i = 0; i < list.size(); i++)
				{
					ss.str("");
					ss << u8"SELECT distinct item_name FROM sn_share where event_name = '" << list[i].event_name.c_str() << "'";
					ImGui::Text(u8"%s", ss.str().c_str());
				}

				ImGui::EndChild();*/
				
			}
			else if (show_copy_child == true)
			{

				ImGui::BeginChild("copy", ImVec2(0, 0), true, main_flags);
				ImGui::Columns(2, "copy_column", true, main_cloumn_flags);
				ImGui::SetColumnWidth(0, 280);
				// Type
				{
					ImGuiComboFlags type_flags = 0;
					type_sel = list[0].event_name.c_str();
					if (ImGui::BeginCombo("type", type_sel, type_flags, false, 265.0f))
					{
						item_inuse = 0;
						for (int n = 0; n < list.size(); n++)
						{
							bool is_selected = (type_sel == list[n].event_name.c_str());
							if (ImGui::Selectable(list[n].event_name.c_str(), is_selected))
							{
								type_sel = list[n].event_name.c_str();
								event_inuse = n;
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
								show_get_button = false;
								show_serial_number = false;
							}
						}
						ImGui::EndCombo();
					}
				}
				// item
				{
					ImGui::BeginChild("item_list", ImVec2(-1, -1), true, share_flag);
					for (int i = 0; i < list[event_inuse].item_name.size(); i++)
					{
						if (ImGui::Button(list[event_inuse].item_name[i].c_str(), ImVec2(-1, 25)))
						{
							show_get_button = true;
							show_serial_number = false;
							std::stringstream ss;
							ss.str("");
							if (show_get_button == true)
							{
								//SELECT IFNULL( (SELECT field1 FROM table WHERE id = 123 LIMIT 1) ,'not found');
								ss << "SELECT  * FROM mabi.sn_share where event_name = '" << list[event_inuse].event_name << "'and item_name = '"
									<< list[event_inuse].item_name[i].c_str() << "' and used = b'0' limit 1;";
								res = stmt->executeQuery(ss.str().c_str());
								while (res->next())
								{
									serial_number = res->getString("serial_number").c_str();
									primary_key = res->getInt("sn_id");
								}
								if (serial_number == last_serial_number)
								{
									serial_number = u8"無剩餘序號";
								}
								else
								{
									last_serial_number = serial_number;
								}
								item_inuse = i;
							}
						}
						//SELECT serial_number FROM mabi.sn_share where event_name = '13th週年慶農場物語' and  item_name = '祝福藥水x10' limit 1;

					}
					ImGui::EndChild();
				}
				ImGui::NextColumn();
				ImGui::BeginChild("SNlist", ImVec2(0, 0), true, main_flags);
				{
					ImGui::Columns(2, "SNlistname_cloumn", true, main_cloumn_flags);
					ImGui::Text(u8"道具名稱");
					ImGui::NextColumn();
					ImGui::Text(u8"序號");
					ImGui::EndColumns();
					ImGui::Separator();
					if (show_get_button || show_serial_number)
					{
						if (!show_serial_number)
						{
							if ((ImGui::Button(u8"領取序號", ImVec2(-1, -1))))
							{
								show_get_button = false;
								show_serial_number = true;
								//UPDATE mabi.`sn_share` SET `used`='1' WHERE `sn_id`='68';
								std::stringstream ss;
								ss << "UPDATE `sn_share` SET `used`=b'1' WHERE `sn_id`=" << primary_key;
								stmt->execute(ss.str().c_str());
							}
						}
						else
						{
							ImGui::Columns(2, "SNlist_cloumn", true, main_cloumn_flags);
							ImGui::Text(u8"%s", list[event_inuse].item_name[item_inuse].c_str());
							ImGui::NextColumn();
							ImGui::Text(u8"%s", serial_number.c_str());
							ImGui::EndColumns();

							if (ImGui::Button("copy", ImVec2(-1, -1)))
							{
								if (serial_number != u8"無剩餘序號")
								{
									OpenClipboard(0);
									EmptyClipboard();
									HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, serial_number.size() + 1);
									memcpy(GlobalLock(hg), serial_number.c_str(), serial_number.size() + 1);
									GlobalUnlock(hg);
									SetClipboardData(CF_TEXT, hg);
									CloseClipboard();
									GlobalFree(hg);
								}

								show_get_button = false;
								show_serial_number = false;
							}
						}
					}



				}
				ImGui::EndChild();
				ImGui::EndChild();

			}
			else if (show_share_child == true)
			{
				static std::vector<std::string> store1;
				static char text[200 * 100];
				if (ImGui::Button(u8"新增序號"))	show_share_text = true;
				ImGui::SameLine();
				if (ImGui::Button(u8"確認") || show_share_text == false)
				{
					show_share_text = false;
					ImGui::SameLine();
					if (ImGui::Button(u8"送出"))	//clear text and send to mysql
					{
						std::string command;
						for (int i = 0; i < store1.size(); i = i + 3)
						{
							//INSERT INTO mabi.`sn_share` (serial_number, event_name, item_name, sn_id) VALUES('G1GWNJ0ZT199HCKNRU', '祝福藥水x10', '13th週年慶農場物語', now());

							try
							{
								command += "INSERT INTO `sn_share` (serial_number, item_name, event_name) VALUES ('";
								command += store1[i + 2];
								command += "', '";
								command += store1[i + 1];
								command += "', '";
								command += store1[i];
								command += "');";
								//std::cout << command << "\n";
								stmt->execute(command.c_str());
								command.clear();
							}
							catch (sql::SQLException &e)
							{
								std::cout << "# ERR:SQLException in" << __FILE__;
								std::cout << "(" << __FUNCTION__ << ") on line "
									<< __LINE__ << "\n";
								std::cout << "# ERR: " << e.what();
								std::cout << " (MySQL error code: " << e.getErrorCode();
								std::cout << ", SQLState: " << e.getSQLState() << " )" << "\n";
								if (e.getErrorCode() == 1062)	std::cout << "same sn \n";
							}
						}
						if (show_share_text == false)
						{
							memset(text, NULL, sizeof(text));
							store1.clear();
						}
					}
				}
				ImGui::BeginChild("share", ImVec2(0, 0), true, share_flag);

				if (show_share_text)
				{
					ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1, -1), ImGuiInputTextFlags_AllowTabInput);

					std::vector<std::string> store;
					std::string str(text);
					std::string sub;
					int count = 0;					// separate string count
					int k = 0;						// separate string start #
					for (unsigned int i = 0; i < str.size(); i++)
					{
						if (str[i] == '\t' || str[i] == '\n')
						{
							std::string sep;
							for (unsigned int j = k; j < i; j++)
							{
								sep += str[j];
							}
							k = i + 1;
							if (count == 0 || count == 1 || count == 2)	//0:SN 1:event_name 2:item_name
							{
								store.push_back(sep);
							}
							count++;
							sep.clear();
						}
						if (count == 6)	count = 0;
					}


					store1 = store;

					for (unsigned int i = 0; i < store.size(); i++)
					{
						const char *testa = store[i].c_str();
						ImGui::Text(testa);
					}
				}
				else
				{
					ImGui::Columns(2, "SNlist_cloumn", true, main_cloumn_flags);
					ImGui::Text(u8"道具名稱");
					ImGui::NextColumn();
					ImGui::Text(u8"序號");
					ImGui::NextColumn();
					ImGui::Separator();

					for (unsigned int i = 0; i < store1.size(); i++)
					{
						if (i % 3 == 1 || i % 3 == 2)
						{
							const char *testa = store1[i].c_str();
							ImGui::Text(testa);
							ImGui::NextColumn();
							if (i % 3 == 2)	ImGui::Separator();
						}
					}
				}
				ImGui::EndChild();
			}


			ImGui::End();
		}

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glEnable(GL_BLEND);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	delete stmt;
	delete con;


	return 0;
}
