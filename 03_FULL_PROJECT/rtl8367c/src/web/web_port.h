
#ifndef __WEB_PORT_H__
#define __WEB_PORT_H__

/* Header files */
#include <web/http_types.h>

/* External functions */
extern int32 get_port(struct http_request_s *pReq);
extern int32 get_portcurrstatus(struct http_request_s *pReq);
extern int32 get_bandwidthcontrolstatus (struct http_request_s *pReq);
extern int32 get_port_bandwidthcontrol(struct http_request_s *pReq);
extern int32 get_port_statistics(struct http_request_s *pReq);
extern int32 get_port_mirroring(struct http_request_s *pReq);
extern int32 get_port_isolation(struct http_request_s *pReq);

extern int32 post_port(struct http_request_s *pReq);
extern int32 post_bandwidthcontrol(struct http_request_s *pReq);
extern int32 post_port_isolation(struct http_request_s *pReq);

extern int32 post_port_mirroring(struct http_request_s *pReq);
extern int32 post_port_mirroring_delete(struct http_request_s *pReq);
extern int32 post_port_statistics_clear(struct http_request_s *pReq);

extern int32 post_port_statistics_rmonQuery(struct http_request_s *pReq);

#endif  /* __WEB_PORT_H__ */


