#include <kernel/log/log.h>
#include <kernel/memory/amm.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/module/module.h>
#include <kernel/socket/port.h>
#include <kernel/socket/socket.h>
#include <kernel/types.h>
#include <kernel/util/memory.h>
#include <kernel/util/util.h>
#include <net/checksum.h>
#include <net/ip4.h>
#include <net/tcp.h>
#define KERNEL_LOG_NAME "net_tcp"



#define PROTOCOL_TYPE 6



static omm_allocator_t* KERNEL_INIT_WRITE _net_tcp_address_allocator=NULL;



static bool _socket_bind_callback(socket_vfs_node_t* socket_node,const void* address,u32 address_length){
	const net_tcp_address_t* tcp_address=(const net_tcp_address_t*)address;
	if (address_length!=sizeof(net_tcp_address_t)||!socket_port_reserve(socket_node,tcp_address->port)){
		return 0;
	}
	net_tcp_address_t* local_ctx=omm_alloc(_net_tcp_address_allocator);
	*local_ctx=*tcp_address;
	socket_node->local_ctx=local_ctx;
	return 1;
}



static void _socket_debind_callback(socket_vfs_node_t* socket_node){
	panic("TCP:_socket_debind_callback");
}



static bool _socket_connect_callback(socket_vfs_node_t* socket_node,const void* address,u32 address_length){
	const net_tcp_address_t* tcp_address=(const net_tcp_address_t*)address;
	if (address_length!=sizeof(net_tcp_address_t)||!socket_port_reserve(socket_node,tcp_address->port)){
		return 0;
	}
	net_tcp_address_t* remote_ctx=omm_alloc(_net_tcp_address_allocator);
	*remote_ctx=*tcp_address;
	socket_node->remote_ctx=remote_ctx;
	return 1;
}



static void _socket_deconnect_callback(socket_vfs_node_t* socket_node){
	panic("TCP:_socket_deconnect_callback");
}



static KERNEL_AWAITS u64 _socket_read_callback(socket_vfs_node_t* socket_node,void* buffer,u64 length,u32 flags){
	socket_packet_t* socket_packet=socket_pop_packet(&(socket_node->node),!(flags&VFS_NODE_FLAG_NONBLOCKING));
	if (!socket_packet){
		return 0;
	}
	WARN("TCP:_socket_read_callback");
	return 0;
}



static u64 _socket_write_callback(socket_vfs_node_t* socket_node,const void* buffer,u64 length){
	if (!socket_node->local_ctx||!socket_node->remote_ctx){
		return 0;
	}
	WARN("TCP:_socket_write_callback");
	return 0;
}



static bool _socket_write_packet_callback(socket_vfs_node_t* socket_node,const void* buffer,u32 length){
	WARN("TCP:_socket_write_packet_callback");
	return 0;
}



static const socket_dtp_descriptor_t _net_tcp_socket_dtp_descriptor={
	"TCP",
	SOCKET_DOMAIN_INET,
	SOCKET_TYPE_STREAM,
	SOCKET_PROTOCOL_TCP,
	NULL,
	_socket_bind_callback,
	_socket_debind_callback,
	_socket_connect_callback,
	_socket_deconnect_callback,
	_socket_read_callback,
	_socket_write_callback,
	_socket_write_packet_callback
};



static void _rx_callback(net_ip4_packet_t* packet){
	WARN("TCP:_rx_callback");
}



static const net_ip4_protocol_descriptor_t _net_tcp_ip4_protocol_descriptor={
	"TCP",
	PROTOCOL_TYPE,
	_rx_callback
};



MODULE_INIT(){
	_net_tcp_address_allocator=omm_init("net.tcp.address",sizeof(net_tcp_address_t),8,4);
	rwlock_init(&(_net_tcp_address_allocator->lock));
}



MODULE_POSTINIT(){
	LOG("Registering TCP protocol...");
	socket_register_dtp_descriptor(&_net_tcp_socket_dtp_descriptor);
	net_ip4_register_protocol_descriptor(&_net_tcp_ip4_protocol_descriptor);
}
