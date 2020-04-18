#ifndef SERVER_H
#define SERVER_H

#include "Ossium.h"

using namespace Ossium;

class Server;

struct ServerSchema : Schema<ServerSchema, 20>
{
    DECLARE_BASE_SCHEMA(ServerSchema, 20);

    M(vector<Server*>, connections);

    M(float, health) = 1.0f;

};

class Server : public GraphicComponent, ServerSchema
{
public:
    CONSTRUCT_SCHEMA(GraphicComponent, ServerSchema);
    DECLARE_COMPONENT(GraphicComponent, Server);

    void OnCreate();

    void Render(Renderer& renderer);

};

#endif // SERVER_H
