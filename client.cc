class Router
{
		public:
				Router();
				init_network();
				add_new_server();
				route();
				rm_server();
				server_hashing();
				client_hashing();
		private:

}
class server
{
		public:
				init_network();
				register_router();
				init_working_thread_pool();
		private:
}
class client
{
		public:
				route_request();
				service_request();
		private:
}
