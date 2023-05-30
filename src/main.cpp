#include "GBM.hpp"
#include <string.h>
#include <algorithm>

#include "stockdata.hpp"
#include "window.h"
#include "glplot.hpp"
#include "readdata.hpp"


void call_error() {
    std::cout<<"Usage"<<std::endl;
    std::cout<<"GBM -i [path to csv file] -o [path to output file] -p [true/false] -samp [number of samples] -days [number of days]"<<std::endl;
    exit(0);
}

int main(int argc, char* argv[]) {
    /*
        * 0 = Home
        * 1 = Plot
    */
    int page = 0;
    double* buffer;
    std::string stock;
    static int samples = 0;
    static int days = 0;
    std::vector<StockData> old_data;
    Window mywindow(1280,720);
    GLplot myplot(mywindow);

    std::cout<<mywindow.isopen()<<std::endl;
    while(mywindow.isopen()) {
        mywindow.before_draw();


        if(page == 0) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowSize(ImVec2(mywindow.getWidth(),mywindow.getHeight()));
            ImGui::SetNextWindowPos(ImVec2(0, 0));

            ImGui::Begin("Stock Data Analysis");

            static bool optionAAPL = false;
            static bool optionMSFT = false;
            static bool optionGOOG = false;
            static bool optionNVDA = false;
            static bool optionINTC = false;

            ImGui::Text("Select Stock:");
            ImGui::Checkbox("INTC", &optionINTC);
            ImGui::Checkbox("AAPL", &optionAAPL);
            ImGui::Checkbox("MSFT", &optionMSFT);
            ImGui::Checkbox("GOOG", &optionGOOG);
            ImGui::Checkbox("NVDA", &optionNVDA);

            ImGui::Text("Number of Samples:");
            ImGui::InputInt("##samples", &samples);

            ImGui::Text("Number of Days:");
            ImGui::InputInt("##days", &days);

            if (ImGui::Button("Start")) {
                if     (optionAAPL == true) stock = "AAPL";
                else if(optionMSFT == true) stock = "MSFT";
                else if(optionGOOG == true) stock = "GOOG";
                else if(optionINTC == true) stock = "INTC";
                else if(optionNVDA == true) stock = "NVDA";

                const int numdata = (days < 1000)?1000 : ((days*2 < 15000)?(days*2):15000);
                old_data = readdata(numdata,stock);
                std::reverse(old_data.begin(),old_data.end());
                buffer = GBM(days,samples,old_data);
                myplot.add_data(buffer,samples,days,old_data);
                page = 1;
            }

            ImGui::End();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        if(page == 1){
            myplot.draw();
        }

        mywindow.swapbuffers(1);
    }
    free(buffer);

    return 0;
}
