typedef struct entities_st{
float a;
float b;
float c;
}entities;


__kernel void vecAdd(__global entities * entity,const int alpha, const int n){
 //Get our global thread ID                                  
    int id = get_global_id(0);                                  
                                                                
    //Make sure we do not go out of bounds                      
    if (id < n)     	                                              
        entity[id].c = alpha*entity[id].a + entity[id].b; 
}

