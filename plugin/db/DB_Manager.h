/*
 * DB_Manager.h
 *
 *  Created on: Nov 9, 2016
 *      Author: zhangyalei
 */

#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_

#include <unordered_set>
#include <unordered_map>
#include "Bit_Buffer.h"
#include "Buffer_List.h"
#include "Object_Pool.h"
#include "Thread.h"
#include "Node_Define.h"

enum Enpoint_Gid {
	GID_DATA_SERVER		= 1,
	GID_DATA_CONNECTOR		= 2,
};

enum Node_Code {
	SELECT_DB_DATA_FAIL			= 1,	//查询db数据失败
	LOAD_DB_DATA_FAIL			= 2,	//加载db数据失败
	SAVE_DB_DATA_FAIL			= 3,	//保存db数据失败
	DELETE_DB_DATA_FAIL			= 4,	//删除db数据失败
	LOAD_RUNTIME_DATA_FAIL		= 5,	//加载运行时数据失败
	SAVE_RUNTIME_DATA_FAIL		= 6,	//保存运行时数据失败
	DELETE_RUNTIME_DATA_FAIL	= 7,	//删除运行时数据失败
	SAVE_DB_DATA_SUCCESS		= 8,	//保存db数据成功
};

enum DB_Msg {
	SYNC_NODE_CODE = 1,
	SYNC_NODE_INFO = 2,
	SYNC_SELECT_DB_DATA = 246,
	SYNC_RES_SELECT_DB_DATA = 247,
	SYNC_GENERATE_ID = 248,
	SYNC_RES_GENERATE_ID = 249,
	SYNC_LOAD_DB_DATA = 250,
	SYNC_SAVE_DB_DATA = 251,
	SYNC_DELETE_DB_DATA = 252,
	SYNC_LOAD_RUNTIME_DATA = 253,
	SYNC_SAVE_RUNTIME_DATA = 254,
	SYNC_DELETE_RUNTIME_DATA = 255,
};

class DB_Manager: public Thread {
	typedef Buffer_List<Mutex_Lock> Data_List;
	typedef std::unordered_map<uint, int> Session_Map;
	typedef std::unordered_set<uint> UInt_Set;
	typedef std::vector<int> Int_Vec;
public:
	static DB_Manager *instance(void);

	int init(const Node_Info &node_info);
	virtual void run_handler(void);
	virtual int process_list(void);

	inline void push_buffer(Byte_Buffer *buffer) {
		notify_lock_.lock();
		buffer_list_.push_back(buffer);
		notify_lock_.signal();
		notify_lock_.unlock();
	}

	//根据条件查询db接口
	void select_db_data(Msg_Head &msg_head, Bit_Buffer &buffer);
	void generate_id(Msg_Head &msg_head, Bit_Buffer &buffer);

	//db数据操作接口
	void load_db_data(Msg_Head &msg_head, Bit_Buffer &buffer);
	void save_db_data(Msg_Head &msg_head, Bit_Buffer &buffer);
	void delete_db_data(Msg_Head &msg_head, Bit_Buffer &buffer);

	//运行时数据操作接口
	void load_runtime_data(Msg_Head &msg_head, Bit_Buffer &buffer);
	void save_runtime_data(Msg_Head &msg_head, Bit_Buffer &buffer);
	void delete_runtime_data(Msg_Head &msg_head, Bit_Buffer &buffer);

private:
	DB_Manager(void);
	virtual ~DB_Manager(void);
	DB_Manager(const DB_Manager &);
	const DB_Manager &operator=(const DB_Manager &);

private:
	static DB_Manager *instance_;

	Data_List buffer_list_;			//消息列表
	Node_Info node_info_;			//节点信息
	Session_Map session_map_;		//转发到connector进程session信息
	UInt_Set sid_set_;				//本进程sid列表
	int data_node_idx_;				//data链接器id索引
	int data_connector_size_;		//data链接器数量
	Int_Vec data_connector_list_;	//data链接器cid列表
	UInt_Set data_fork_list_;		//data进程启动列表
};

#define DB_MANAGER DB_Manager::instance()

#endif /* DB_MANAGER_H_ */
