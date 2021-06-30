#pragma once

#include <chrono>

//Repository+Entity 数据操作层
class VirtualWalletEntity
{
public:
	VirtualWalletEntity()
	{
	}

	~VirtualWalletEntity()
	{
	}

	long long id;
	double balance;
private:
};
class VirtualWalletRepository	
{
public:
	VirtualWalletRepository()
	{
	}

	~VirtualWalletRepository()
	{
	}

	VirtualWalletEntity GetWalletById(long long id);
	int UpdataBalance(long long id, double amount);

private:

};

enum TransactionStatus
{
	TO_BE_EXECUTE,
	CLOSED,
	FAILED,
	EXECUTE,
};
class TransactionEntity
{
public:
	TransactionEntity()
	{
	}

	~TransactionEntity()
	{
	}

	double amount;
	long long create_time;
	long long from_wallet_id;
	long long to_wallet_id;
	long long transcation_id;
	TransactionStatus status;
private:
};
class TransactionRepository
{
public:
	TransactionRepository()
	{
	}

	~TransactionRepository()
	{
	}

	long long SaveTransaction(TransactionEntity tran_entity);
	int UpdataTransactionStatus(long long id, TransactionStatus status);

private:

};


// Service+BO(Business Object)	业务逻辑处理层
class VirtualWalletDomain	
{
public:
	VirtualWalletDomain()
	{
	}

	~VirtualWalletDomain()
	{
	}

private:
	long long id;
	double balance;
};
class VirtualWalletService
{
public:
	VirtualWalletService()
	{
	}

	~VirtualWalletService()
	{
	}

	double GetBalance(long long id)
	{
		VirtualWalletEntity wallet = virtual_wallet_repo.GetWalletById(id);
		return wallet.balance;
	}
	int Debit(long long id, double amount)
	{
		int ret = 0;
		VirtualWalletEntity wallet = virtual_wallet_repo.GetWalletById(id);
		double balance = wallet.balance;
		
		TransactionEntity trans_entity;
		trans_entity.amount = amount;
		trans_entity.create_time = std::chrono::steady_clock::now().time_since_epoch().count();
		trans_entity.from_wallet_id = id;
		trans_entity.status = TO_BE_EXECUTE;
		long long trans_id = trans_repo.SaveTransaction(trans_entity);

		if (0 != virtual_wallet_repo.UpdataBalance(id, balance - amount))
		{
			trans_repo.UpdataTransactionStatus(trans_id, CLOSED);
			ret = -1;
		}
		else
		{
			trans_repo.UpdataTransactionStatus(trans_id, EXECUTE);
		}
		return ret;
	}
	int Credit(long long id, double amount)
	{
		int ret = 0;
		VirtualWalletEntity wallet = virtual_wallet_repo.GetWalletById(id);
		double balance = wallet.balance;
		
		TransactionEntity trans_entity;
		trans_entity.amount = amount;
		trans_entity.create_time = std::chrono::steady_clock::now().time_since_epoch().count();
		trans_entity.to_wallet_id = id;
		trans_entity.status = TO_BE_EXECUTE;
		long long trans_id = trans_repo.SaveTransaction(trans_entity);

		if (0 != virtual_wallet_repo.UpdataBalance(id, balance + amount))
		{
			trans_repo.UpdataTransactionStatus(trans_id, CLOSED);
			ret = -1;
		}
		else
		{
			trans_repo.UpdataTransactionStatus(trans_id, EXECUTE);
		}
		return ret;
	}
	int Transfer(long long from_id, long long to_id, double amount)
	{
		int ret = 0;

		TransactionEntity trans_entity;
		trans_entity.amount = amount;
		trans_entity.create_time = std::chrono::steady_clock::now().time_since_epoch().count();
		trans_entity.from_wallet_id = from_id;
		trans_entity.to_wallet_id = to_id;
		trans_entity.status = TO_BE_EXECUTE;
		long long trans_id = trans_repo.SaveTransaction(trans_entity);

		if (0 != Debit(from_id, amount)
			|| 0 != Credit(to_id, amount))
		{
			trans_repo.UpdataTransactionStatus(trans_id, CLOSED);
			ret = -1;
		}
		else
		{
			trans_repo.UpdataTransactionStatus(trans_id, EXECUTE);
		}

		return ret;
	}

private:
	VirtualWalletRepository virtual_wallet_repo;
	TransactionRepository trans_repo;
};

// Controller+VO(View Object)	接口暴露层
class VirtualWalletVo
{
public:
	VirtualWalletVo()
	{
	}

	~VirtualWalletVo()
	{
	}

private:

};
class VirtualWalletController
{
public:
	VirtualWalletController()
	{
	}

	~VirtualWalletController()
	{
	}

	double GetBalance(long long wallet_id) { virtual_wallet_service.GetBalance(wallet_id); }
	double Debit(long long wallet_id, double amount) { virtual_wallet_service.Debit(wallet_id, amount); }
	double Credit(long long wallet_id, double amount) { virtual_wallet_service.Credit(wallet_id, amount); }
	double Transfer(long long from_wallet_id, long long to_wallet_id, double amount) { virtual_wallet_service.Transfer(from_wallet_id, to_wallet_id, amount); }

private:
	VirtualWalletService virtual_wallet_service;
};

