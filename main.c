#include <stdbool.h>
#include "include/SDL.h"
#include <stdio.h>
#include <stdlib.h>

#include "construct_qr_code.h"

void draw_qr_code(int qr_code_size, bool **qr_code)
{
    SDL_Window *window = SDL_CreateWindow("qr code", 100, 100, 630, 630, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event event_;
    while (true) {
        SDL_PollEvent(&event_);
        if (event_.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            break;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int i = 0; i < qr_code_size; ++i) {
            for (int j = 0; j < qr_code_size; ++j) {
                if (qr_code[i][j]) {
                    SDL_Rect rect = { 30 + j * 21, 30 + i * 21, 21, 21 };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }
}

extern int encode_message_v1M(int, char *, int, bool *);

int main(int argc, char **argv)
{
    bool **qr_code = construct_qr_code_v1M(10, "GOOGLE.COM");   
    draw_qr_code(V1M_QR_CODE_SIZE, qr_code);
    
    return 0;
}
