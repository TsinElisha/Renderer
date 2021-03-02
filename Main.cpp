//
// Created by tsin on 2021/3/2.
//
#include "tgaimage.h"

int main(){
    TGAImage img(100, 100, 4);
    TGAColor clr(100, 100, 100);
    img.set(50, 50, clr);
    img.write_tga_file("out.tga");

}