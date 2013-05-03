// DacType.h

#ifndef _PPBOX_DAC_DAC_TYPE_H_
#define _PPBOX_DAC_DAC_TYPE_H_

namespace ppbox
{
    namespace dac
    {

        struct InterfaceType
        {
            enum Enum
            {
                none,
                play_open_info_type,
                play_close_info_type,
                auth_info_type,
                mutex_info_type,
                run_info_type,
                terminate_info_type,
                sys_info_type,
                peer_info_type,

                log_info_type, 
            };
        };

        struct LogReason
        {
            enum Enum
            {
                open_too_long,
                open_fail,
                auth_mutex_fail,
                proc_fail,
                ui_check,
                user_define,

                none = 1024, 
            };
        };

        struct StbType
        {
            enum Enum
            {
                intermediate_layer = 10,    // ÷–º‰≤„
                adaptation_layer = 11       //   ≈‰≤„
            };
        };

        struct TerminateType
        {
            enum Enum
            {
                auth_error = 1,
                mutex_kickout_packet,
                mutex_not_connect,
                mutex_time_out,
                auth_time_out,
                mutex_mev_failed,
                other_error
            };
        };

        static boost::system::errc::errc_t const ErrorTypes[] = {
            boost::system::errc::success, 
            boost::system::errc::operation_not_permitted,
            boost::system::errc::no_such_file_or_directory,
            boost::system::errc::no_such_process,
            boost::system::errc::interrupted,
            boost::system::errc::io_error,
            boost::system::errc::no_such_device_or_address,
            boost::system::errc::argument_list_too_long,
            boost::system::errc::executable_format_error,
            boost::system::errc::bad_file_descriptor,
            boost::system::errc::no_child_process,
            boost::system::errc::resource_unavailable_try_again,
            boost::system::errc::not_enough_memory,
            boost::system::errc::permission_denied,
            boost::system::errc::bad_address,
            boost::system::errc::device_or_resource_busy,
            boost::system::errc::file_exists,
            boost::system::errc::cross_device_link,
            boost::system::errc::no_such_device,
            boost::system::errc::not_a_directory,
            boost::system::errc::is_a_directory,
            boost::system::errc::invalid_argument,
            boost::system::errc::too_many_files_open_in_system,
            boost::system::errc::too_many_files_open,
            boost::system::errc::inappropriate_io_control_operation,
            boost::system::errc::text_file_busy,
            boost::system::errc::file_too_large,
            boost::system::errc::no_space_on_device,
            boost::system::errc::invalid_seek,
            boost::system::errc::read_only_file_system,
            boost::system::errc::too_many_links,
            boost::system::errc::broken_pipe,
            boost::system::errc::argument_out_of_domain,
            boost::system::errc::result_out_of_range,
            boost::system::errc::resource_deadlock_would_occur,
            boost::system::errc::filename_too_long,
            boost::system::errc::no_lock_available,
            boost::system::errc::function_not_supported,
            boost::system::errc::directory_not_empty,
            boost::system::errc::too_many_synbolic_link_levels,
            boost::system::errc::operation_would_block,
            boost::system::errc::no_message,
            boost::system::errc::identifier_removed,
            boost::system::errc::not_a_stream,
            boost::system::errc::no_message_available,
            boost::system::errc::stream_timeout,
            boost::system::errc::no_stream_resources,
            boost::system::errc::no_link,
            boost::system::errc::protocol_error,
            boost::system::errc::bad_message,
            boost::system::errc::value_too_large,
            boost::system::errc::illegal_byte_sequence,
            boost::system::errc::not_a_socket,
            boost::system::errc::destination_address_required,
            boost::system::errc::message_size,
            boost::system::errc::wrong_protocol_type,
            boost::system::errc::no_protocol_option,
            boost::system::errc::protocol_not_supported,
            boost::system::errc::operation_not_supported,
            boost::system::errc::address_family_not_supported,
            boost::system::errc::address_in_use,
            boost::system::errc::address_not_available,
            boost::system::errc::network_down,
            boost::system::errc::network_unreachable,
            boost::system::errc::network_reset,
            boost::system::errc::connection_aborted,
            boost::system::errc::connection_reset,
            boost::system::errc::no_buffer_space,
            boost::system::errc::already_connected,
            boost::system::errc::not_connected,
            boost::system::errc::timed_out,
            boost::system::errc::connection_refused,
            boost::system::errc::host_unreachable,
            boost::system::errc::connection_already_in_progress,
            boost::system::errc::operation_in_progress,
            boost::system::errc::operation_canceled,
            boost::system::errc::owner_dead,
            boost::system::errc::state_not_recoverable,
            boost::system::errc::not_supported,
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_TYPE_H_
