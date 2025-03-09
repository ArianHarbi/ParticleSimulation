#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h> 
#include <SDL3/SDL_main.h> 
#include <vector>
#include <cmath>
#include <string>

using namespace std;

static SDL_Window *window = NULL; 
static SDL_Renderer *renderer = NULL; 
const int WIDTH = 1000, HEIGHT = 600;
const float GRAVITY = 0.05;
static int CHARGE = -1;
const double EMASS = 1e-30, NPMASS = 17e-30, KCONST = 9e9, e = 0.8;
float mouseX,mouseY;
static int pCount = 0,eCount = 0 , nCount = 0, prCount = 0;
bool FOF = true, VFOF = true , HEFD = true, GOF = true, MOF = true , MFD = true, VEFD = true, VOHEF = true;
string ON = "ON" , OFF = "OFF";

class Mfield{
    double B=1e-12;
    float x,y;
    float FW,FH;
    SDL_FRect MF;
public:
    Mfield(){
        x = WIDTH/2;
        y = HEIGHT/2;
        FW = 200, FH = 200;
    }void set_XY(float X, float Y){
        x = (X > FW/2 && X < WIDTH-FW/2) ? X : x;
        y = (Y > FH/2 && Y < HEIGHT-FH/2) ? Y: y;
    }void RenderField(){
        MF.h = FH;
        MF.w = FW;
        MF.x = x - FW/2;
        MF.y = y - FH/2;
        SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer,&MF);
    }float getX(){
        return x;
    }float getY(){
        return y;
    }float getFW(){
        return FW;
    }float getFH(){
        return FH;
    }double getB(){
        return B;
    }void setFWFH(int fw, int fh){
        FW = fw;
        FH = fh;
    }
};

class field{
    double E=0.9e-12;
    float x,y;
    float FW,FH;
public:
    field(){
        x = WIDTH /2;
        y = HEIGHT/2;
        FW = 200 , FH = 200;
    }
    void set_XY(float X,float Y){
        x = (X > FW/2 && X < WIDTH-FW/2) ? X : x;
        y = (Y > FH/2 && Y < HEIGHT-FH/2) ? Y: y;
    }void RenderField(){
        SDL_FRect R1,R2;
        R1.h = FH , R1.w = 5 , R1.x = x-5-FW/2 , R1.y = y-FH/2 ;
        R2.h = FH , R2.w = 5 , R2.x = x+FW/2 , R2.y = y-FH/2 ;
        if(HEFD){
            SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R1);
            SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R2);
        }else{
            SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R1);
            SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R2);
        }
    }void RenderFieldV(){
        SDL_FRect R1,R2;
        R1.h = 5 , R1.w = FW , R1.x = x-FW/2 , R1.y = y-5-FH/2 ;
        R2.h = 5 , R2.w = FW , R2.x = x-FW/2 , R2.y = y+FH/2 ;
        if(VEFD){
            SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R1);
            SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R2);
        }else{
            SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R1);
            SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&R2);
        }
    }float getX(){
        return x;
    }float getY(){
        return y;
    }float getFW(){
        return FW;
    }float getFH(){
        return FH;
    }double getE(){
        return E;
    }void setFWFH(int fw, int fh){
        FW = fw;
        FH = fh;
    }
};

class particle{
    double x,y;
    double ay=0,ax=0,ag=0;
    double vx=0,vy=0,vg=0;
    double charge = 1.6e-19;
    double mass;
public:
    particle(int Charge, float X, float Y){
        charge *= Charge;
        x = X;
        y = Y;
        if(charge < 0) mass = EMASS;
        else mass = NPMASS;
    }void RenderParticle(SDL_Renderer * renderer){
        if(charge < 0) SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
        else if(charge > 0) SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
        else SDL_SetRenderDrawColor(renderer,0,0,255,SDL_ALPHA_OPAQUE);
        /*if(charge < 0) {
            SDL_RenderPoint(renderer,x,y);
            SDL_RenderPoint(renderer,x+1,y);
            SDL_RenderPoint(renderer,x,y+1);
            SDL_RenderPoint(renderer,x+1,y+1);
        }*/
        //else {
            SDL_RenderPoint(renderer,x,y);
            SDL_RenderPoint(renderer,x+1,y);
            SDL_RenderPoint(renderer,x,y+1);
            SDL_RenderPoint(renderer,x+1,y+1);
            SDL_RenderPoint(renderer,x-1,y);
            SDL_RenderPoint(renderer,x,y-1);
            SDL_RenderPoint(renderer,x-1,y-1);
            SDL_RenderPoint(renderer,x+1,y-1);
            SDL_RenderPoint(renderer,x-1,y+1);
        //}
        
    }void update() {
        if(GOF) vg += ag;
        //else vg = 0;
        vx += ax;
        vy += ay;
        x += vx;
        y += vy + vg;
        if (x < 0) {
            x = WIDTH;
            vx *= e;
            //vx *= -1;
        }else if (x > WIDTH) {
            x = 0; 
            vx *= e;
            //vx *= -1;
        }if (y < 0) {
            y = 0;
            vy *= e;
            vy *= -1;
            vg *= e;
            vg *= -1;
        } else if (y > HEIGHT -5) {
            y = HEIGHT - 5;
            vy *= e;
            vy *= -1;
            vg *= e;
            vg *= -1;        
        }
    }void set_accel(double AX, double AY, double AG){
        ax = AX;
        ay = AY;
        ag = AG;
    }double get_charge(){
        return charge;
    }float get_x(){
        return x;
    }float get_y(){
        return y;
    }double get_mass(){
        return mass;
    }double get_vy(){
        return vy;
    }double get_vx(){
        return vx;
    }
};

vector<particle> allPartics;
field PrimaryF;
field SecondaryF;
Mfield PrimaryMF;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
    if(argc >= 3){
        if(stoi(argv[1]) <= WIDTH-5 && stoi(argv[2]) <= HEIGHT-5){
            PrimaryF.setFWFH(stoi(argv[1]),atoi(argv[2]));
            PrimaryMF.setFWFH(stoi(argv[1]),stoi(argv[2]));
            SecondaryF.setFWFH(stoi(argv[1]),stoi(argv[2]));
        }
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_Log("failed to initilize SDL: %s" , SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (SDL_CreateWindowAndRenderer("Pong", WIDTH, HEIGHT, 0, &window, &renderer) < 0){
        SDL_Log("failed to initilize window and rebderer: %s" ,SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    return SDL_APP_CONTINUE;
} 

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){ 
    
    if (event->type == SDL_EVENT_QUIT) { 
        return SDL_APP_SUCCESS;  
    }else if(event->type == SDL_EVENT_KEY_UP){
        if(event->key.scancode == SDL_SCANCODE_E){
            CHARGE = -1;
        }else if(event->key.scancode == SDL_SCANCODE_P){
            CHARGE = 1;
        }else if(event->key.scancode == SDL_SCANCODE_N){
            CHARGE = 0;
        }else if(event->key.scancode == SDL_SCANCODE_F){
            FOF = (FOF == true) ? false : true ; 
        }else if(event->key.scancode == SDL_SCANCODE_G){
            GOF = (GOF == true) ? false : true ; 
        }else if(event->key.scancode == SDL_SCANCODE_M){
            MOF = (MOF == true) ? false : true ;
        }else if(event->key.scancode == SDL_SCANCODE_B){
            MFD = (MFD == true) ? false : true ; 
        }else if(event->key.scancode == SDL_SCANCODE_C){
            HEFD = (HEFD == true) ? false : true ;
        }else if(event->key.scancode == SDL_SCANCODE_H){
            VOHEF = (VOHEF == true) ? false : true ;
        }else if(event->key.scancode == SDL_SCANCODE_V){
            VFOF = (VFOF == true) ? false : true ;
        }else if(event->key.scancode == SDL_SCANCODE_U){
            VEFD = (VEFD == true) ? false : true ;
        }else if(event->key.scancode == SDL_SCANCODE_R){
            for(int i = allPartics.size()-1; i >=0  ; i--){
                allPartics.erase(allPartics.begin()+i);
            }
            prCount=allPartics.size();
            eCount=0 , pCount = 0 , nCount = 0; 
            FOF = true;
            GOF = true;
            MOF = true;
            MFD = true;
            VFOF = true;
            HEFD = true;
            VEFD  = true;
            VOHEF = true;
            PrimaryF.set_XY(WIDTH/2 , HEIGHT/2);
            PrimaryMF.set_XY(WIDTH/2 , HEIGHT/2);
            SecondaryF.set_XY(WIDTH/2 , HEIGHT/2);
        }
    }else if(event->type == SDL_EVENT_KEY_DOWN){
        if(VOHEF){
            if(event->key.scancode == SDL_SCANCODE_A){
            PrimaryF.set_XY(PrimaryF.getX()-5,PrimaryF.getY());
            }else if(event->key.scancode == SDL_SCANCODE_D){
            PrimaryF.set_XY(PrimaryF.getX()+5,PrimaryF.getY());
            }else if(event->key.scancode == SDL_SCANCODE_S){
            PrimaryF.set_XY(PrimaryF.getX(),PrimaryF.getY()+5);
            }else if(event->key.scancode == SDL_SCANCODE_W){
            PrimaryF.set_XY(PrimaryF.getX(),PrimaryF.getY()-5);
            }
        }else{
            if(event->key.scancode == SDL_SCANCODE_A){
            SecondaryF.set_XY(SecondaryF.getX()-5,SecondaryF.getY());
            }else if(event->key.scancode == SDL_SCANCODE_D){
            SecondaryF.set_XY(SecondaryF.getX()+5,SecondaryF.getY());
            }else if(event->key.scancode == SDL_SCANCODE_S){
            SecondaryF.set_XY(SecondaryF.getX(),SecondaryF.getY()+5);
            }else if(event->key.scancode == SDL_SCANCODE_W){
            SecondaryF.set_XY(SecondaryF.getX(),SecondaryF.getY()-5);
            }
        }if(event->key.scancode == SDL_SCANCODE_LEFT){
            PrimaryMF.set_XY(PrimaryMF.getX()-5,PrimaryMF.getY());
        }else if(event->key.scancode == SDL_SCANCODE_RIGHT){
            PrimaryMF.set_XY(PrimaryMF.getX()+5,PrimaryMF.getY());
        }else if(event->key.scancode == SDL_SCANCODE_DOWN){
            PrimaryMF.set_XY(PrimaryMF.getX(),PrimaryMF.getY()+5);
        }else if(event->key.scancode == SDL_SCANCODE_UP){
            PrimaryMF.set_XY(PrimaryMF.getX(),PrimaryMF.getY()-5);
        }
    }else if(event->type == SDL_EVENT_MOUSE_BUTTON_UP){
        SDL_GetMouseState(&mouseX,&mouseY);
        particle newParticle(CHARGE,mouseX,mouseY);
        allPartics.push_back(newParticle);
        prCount++;
        if(CHARGE < 0) eCount++;
        else if(CHARGE > 0) pCount++;
        else nCount++;
    }
    return SDL_APP_CONTINUE;  
} 

SDL_AppResult calculateForces(int ind){
    double forcesX=0,forcesY=0,forceG=0;
    float theta;
    for(int i = 0 ; i < prCount ; i++){
        if (i != ind){
            float distance = sqrt((pow(allPartics.at(ind).get_x() - allPartics.at(i).get_x(),2))+(pow(allPartics.at(ind).get_y() - allPartics.at(i).get_y() , 2)));
            if(distance < 5) distance = 5;
            theta = atan2((allPartics.at(ind).get_y() - allPartics.at(i).get_y()),allPartics.at(ind).get_x() - allPartics.at(i).get_x());
            forcesX += cos(theta)*((KCONST * allPartics.at(ind).get_charge() * allPartics.at(i).get_charge())/pow(distance , 2));
            forcesY += sin(theta)*((KCONST * allPartics.at(ind).get_charge() * allPartics.at(i).get_charge())/pow(distance , 2));
        }
    }
    if(FOF){
        if( allPartics.at(ind).get_x() > PrimaryF.getX()-PrimaryF.getFW()/2 &&
            allPartics.at(ind).get_x() < PrimaryF.getX()+PrimaryF.getFW()/2 &&
            allPartics.at(ind).get_y() > PrimaryF.getY()-PrimaryF.getFH()/2 && 
            allPartics.at(ind).get_y() < PrimaryF.getY()+PrimaryF.getFH()/2){
                int dir = (HEFD) ? 1 : -1;
                forcesX += PrimaryF.getE()*allPartics.at(ind).get_charge()*dir;
            }
    }if(VFOF){
        if( allPartics.at(ind).get_x() > PrimaryF.getX()-PrimaryF.getFW()/2 &&
            allPartics.at(ind).get_x() < PrimaryF.getX()+PrimaryF.getFW()/2 &&
            allPartics.at(ind).get_y() > PrimaryF.getY()-PrimaryF.getFH()/2 && 
            allPartics.at(ind).get_y() < PrimaryF.getY()+PrimaryF.getFH()/2){
                int dir = (VEFD) ? 1 : -1;
                forcesY += PrimaryF.getE()*allPartics.at(ind).get_charge()*dir;
            }
    }if(MOF){
        if( allPartics.at(ind).get_x() > PrimaryMF.getX()-PrimaryMF.getFW()/2 &&
            allPartics.at(ind).get_x() < PrimaryMF.getX()+PrimaryMF.getFW()/2 &&
            allPartics.at(ind).get_y() > PrimaryMF.getY()-PrimaryMF.getFH()/2 && 
            allPartics.at(ind).get_y() < PrimaryMF.getY()+PrimaryMF.getFH()/2)
            {
                if(MFD){
                    forcesY += (allPartics.at(ind).get_charge() * allPartics.at(ind).get_vx() * PrimaryMF.getB()) * -1;
                    forcesX +=  allPartics.at(ind).get_charge() * allPartics.at(ind).get_vy() * PrimaryMF.getB();
                }else{
                    forcesY +=  allPartics.at(ind).get_charge() * allPartics.at(ind).get_vx() * PrimaryMF.getB();
                    forcesX += (allPartics.at(ind).get_charge() * allPartics.at(ind).get_vy() * PrimaryMF.getB()) * -1;
                }
            }
    }
    if(GOF) {
        forceG += allPartics.at(ind).get_mass() * GRAVITY;
    }//else {
    //    forceG = 0;
    //}
    allPartics.at(ind).set_accel(forcesX/(allPartics.at(ind).get_mass()),forcesY/(allPartics.at(ind).get_mass()),forceG/(allPartics.at(ind).get_mass()));
    return SDL_APP_SUCCESS;
}

SDL_AppResult RenderText(){
    SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
    if(FOF) SDL_RenderDebugText(renderer,5,5,"H.E.FIELD : ON");
    else {
        SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
        SDL_RenderDebugText(renderer,5,5,"H.E.FIELD : OFF");
    }
    SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
    if(VFOF) SDL_RenderDebugText(renderer,5,15,"V.E.FIELD : ON");
    else {
        SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
        SDL_RenderDebugText(renderer,5,15,"V.E.FIELD : OFF");
    }
    SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
    if(GOF) SDL_RenderDebugText(renderer,5,25,"GRAVITY : ON");
    else {
        SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
        SDL_RenderDebugText(renderer,5,25,"GRAVITY : OFF");
    }
    SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
    if(MOF) {
        SDL_RenderDebugText(renderer,5,35,"MFIELD : ON");
        if(MFD){
            SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);
            SDL_RenderDebugText(renderer,PrimaryMF.getX(),PrimaryMF.getY(),"+");
        }else{
            SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);
            SDL_RenderDebugText(renderer,PrimaryMF.getX(),PrimaryMF.getY(),"-");
        }
    }else {
        SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
        SDL_RenderDebugText(renderer,5,35,"MFIELD : OFF");
    }
    
    
    SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(renderer,5,45,"E : %d",eCount);
    SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(renderer,5,55,"P : %d",pCount);
    SDL_SetRenderDrawColor(renderer,0,0,255,SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(renderer,5,65,"N : %d",nCount);
    SDL_RenderPresent(renderer);
    return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppIterate(void *appstate){
    SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    if(MOF) PrimaryMF.RenderField();
    if(FOF) PrimaryF.RenderField();
    if(VFOF) SecondaryF.RenderFieldV();
    SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_OPAQUE);
    SDL_FRect Ground;
    Ground.h = 5;
    Ground.w = WIDTH;
    Ground.x = 0;
    Ground.y = HEIGHT - Ground.h;
    SDL_RenderFillRect(renderer,&Ground);
    if(allPartics.size()>0){
        for(int i = 0 ; i < prCount ; i++){
            calculateForces(i);
            allPartics.at(i).update();
            allPartics.at(i).RenderParticle(renderer);
        }
    }
    RenderText();
    SDL_Delay(17);
    return SDL_APP_CONTINUE;
} 

void SDL_AppQuit(void *appstate, SDL_AppResult result){ 
    if(window) SDL_DestroyWindow(window); 
    if (renderer) SDL_DestroyRenderer(renderer); 
    exit(0);
}