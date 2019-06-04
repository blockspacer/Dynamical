#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

class Device;

class MainRender {
public:
    MainRender(Device& device);
    void init();
    ~MainRender();
    void render();
    
private:
    Device& device;
    
};

#endif
