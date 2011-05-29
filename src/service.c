
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <mcast.h>
#include <plugins.h>
#include <service.h>


int
start_service()
{
	// the hwmultd server is a multicast client and
	// the hwmultd client is a multicast server
	if(server_mode == SERVER_MODE)
	{
		if( !already_init_hw )
		{
			if( !((*init_hw)()) )
			{
				write_log(CRIT, "initialized hardware failed");
				clean_exit();
			}
			else
			{
				write_log(DBUG, "initialized hardware");
				already_init_hw = 1;
			}
		}
		else
			write_log(DBUG, "already initialized hardware");

		if( !mclient_start() )
		{
			write_log(ERRO, "mclient_start() failed");
			return 0;
		}
		else
			write_log(DBUG, "mclient_start()");
	}
	else
	{
		// We are switching from server to client, so close hw
		if( already_init_hw )
		{
			write_log(INFO, "switching from server to client and closing already initialized hardware");
			if(!(*close_hw)())
			{
				write_log(ERRO, "failed closed hardware");
				return 0;
			}
			else
			{
				already_init_hw = 0;
				write_log(DBUG, "closed hardware");
			}
		}

		if( !mserver_start() )
		{
			write_log(ERRO, "mserver_start() failed");
			return 0;
		}
		else
			write_log(DBUG, "mserver_start()");
	}

	continue_big_loop = 1;

	return 1;
}


int
do_service()
{
	char *msg;

	while(continue_big_loop)
	{
		sleep(timing);
		if(server_mode == SERVER_MODE)
		{
			msg = read_hw();
			if( !snd_mcast_msg(msg) )
				return 0;
		}
		else
		{
			if( !(msg = rcv_mcast_msg()) )
				return 0;
		}
	}

	if( !((*reset_hw)()) )
	{
		write_log(CRIT, "reset hardware failed");
		clean_exit();
	}
	else
		write_log(DBUG, "reset hardware");

	return 1;
}


int
stop_service()
{
	write_log(DBUG, "stopping service");
	if(server_mode == SERVER_MODE)
		mclient_stop();
	else
		mserver_stop();

	return 1;
}
