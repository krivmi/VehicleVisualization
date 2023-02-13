#include "tools.h"

template <typename T>
int get_index(long id, QVector <T*> messages){
    int stationIndex = -1;

    for(int i = 0; i < messages.size(); i++){
        if(messages.at(i)->stationID == id){
            stationIndex = i;
            break;
        }
    }
    // if the station was not found, return -1
    return stationIndex;
}
