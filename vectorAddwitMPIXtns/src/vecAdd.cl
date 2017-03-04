typedef struct entities_st{
float x;
float y;
float z;
}entities;


__kernel void vecAdd(__global entities * entity,const int alpha, const int n){
 //Get our global thread ID                                  
    int id = get_global_id(0);                                  
                                                                
    //Make sure we do not go out of bounds                      
    if (id < n)     	                                              
        entity[id].z = alpha*entity[id].x + entity[id].y; 
}

