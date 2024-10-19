#include "structs.h"


std::ostream &operator<<(std::ostream &s, event &e)
{
    if (e.is_send_event)
        s << "Send ";
    else
        s << "Read ";
    s << "event("
      << "pid="
      << e.pid
      << ",p_eid="
      << e.p_eid
      << ",msg_id="
      << e.msg_id
      << ",sender_pid="
      << e.sender_pid
      << ")";
    return s;
}