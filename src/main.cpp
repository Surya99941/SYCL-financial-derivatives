#include "GBM.hpp"
#include <string.h>
#include <algorithm>
#include <future>

#include "imgui.h"
#include "stockdata.hpp"
#include "window.h"
#include "glplot.hpp"
#include "readdata.hpp"


void call_error() {
    std::cout<<"Usage"<<std::endl;
    std::cout<<"GBM -i [path to csv file] -o [path to output file] -p [true/false] -samp [number of samples] -days [number of days]"<<std::endl;
    exit(0);
}
static int TextEditCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
    {
        // Handle backspace key
        if (data->EventKey == ImGuiKey_Backspace && data->CursorPos == data->SelectionStart)
        {
            // Delete the character before the cursor
            if (data->CursorPos > 0)
            {
                data->DeleteChars(data->CursorPos - 1, 1);
                data->CursorPos--;
                data->SelectionStart--;
            }
        }
    }
    return 0;
}


int main(int argc, char* argv[]) {
    /*
        * 0 = Home
        * 1 = Plot
    */
    int page = 0;
    double* buffer = NULL;
    char searchBuffer[20];
    std::string stock;
    static int samples = 0;
    static int days = 0;
    std::vector<StockData> old_data;
    Window mywindow(1280,720);
    GLplot myplot(mywindow);
    std::string prev_search = "";
    bool isplot = false;
/*
    std::vector<std::string> search_result;
    bool search_done = true;
    std::future<void> search_future;
    std::string selected;
*/
    while(mywindow.isopen()) {
        mywindow.before_draw();


        if(page == 0) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowSize(ImVec2(mywindow.getWidth(),mywindow.getHeight()));
            ImGui::SetNextWindowPos(ImVec2(0, 0));

            ImGui::Begin("Stock Data Analysis");


            ImGui::Text("Select Stock:");

            ImGui::Text("Number of Samples:");
            ImGui::InputInt("##samples", &samples);

            ImGui::Text("Number of Days:");
            ImGui::InputInt("##days", &days);

            ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer),ImGuiInputTextFlags_CallbackEdit, &TextEditCallback);
            std::string sbuf = std::string(searchBuffer);

            ImGui::Checkbox("Should show Price Paths", &isplot);

/*
            if(search_done && *searchBuffer != '\0'){
                if (ImGui::BeginCombo("##SearchDropdown", selected.c_str())) {
                    for (const auto& item : search_result) {
                        bool isSelected = (item == sbuf);
                        if (ImGui::Selectable(item.c_str(), isSelected)) {
                            selected = item;
                            strcpy(searchBuffer,selected.c_str());
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                if(sbuf != prev_search && sbuf.length()>1){
                    prev_search = sbuf;
                    search_done = false;
                    search_result = {};
                    search_future = std::async(std::launch::async, related, sbuf, &search_result, &search_done);
                }
            }
*/
            if (ImGui::Button("Start")) {
                stock = sbuf;
                const int numdata = (days < 1000)?1000 : ((days*2 < 15000)?(days*2):15000);
                ImGui::Text("Reading Data");
                old_data = readdata(numdata,stock);
                std::reverse(old_data.begin(),old_data.end());
                
                ImGui::Text("Making Predictions");
                buffer = GBM(days,samples,old_data);
                ImGui::Text("Plotting");
                myplot.add_data(buffer,samples,days,old_data,isplot);
                page = 1;
            }

            ImGui::End();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        if(page == 1){
            if(myplot.draw() == false){
                page = 0;
                free(buffer);
                buffer = NULL;
            }
        }

        mywindow.swapbuffers(1);
    }
    if(buffer != NULL)  free(buffer);

    return 0;
}
