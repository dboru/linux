#ifndef _NET_NF_TABLES_OFFLOAD_H
#define _NET_NF_TABLES_OFFLOAD_H

#include <net/flow_offload.h>
#include <net/netfilter/nf_tables.h>

struct nft_offload_reg {
	u32		key;
	u32		len;
	u32		base_offset;
	u32		offset;
	struct nft_data	mask;
};

enum nft_offload_dep_type {
	NFT_OFFLOAD_DEP_UNSPEC	= 0,
	NFT_OFFLOAD_DEP_NETWORK,
	NFT_OFFLOAD_DEP_TRANSPORT,
};

struct nft_offload_ctx {
	struct {
		enum nft_offload_dep_type	type;
		__be16				l3num;
		u8				protonum;
	} dep;
	unsigned int				num_actions;
	struct nft_offload_reg			regs[NFT_REG32_15 + 1];
};

void nft_offload_set_dependency(struct nft_offload_ctx *ctx,
				enum nft_offload_dep_type type);
void nft_offload_update_dependency(struct nft_offload_ctx *ctx,
				   const void *data, u32 len);

struct nft_flow_key {
	struct flow_dissector_key_basic			basic;
	union {
		struct flow_dissector_key_ipv4_addrs	ipv4;
		struct flow_dissector_key_ipv6_addrs	ipv6;
	};
	struct flow_dissector_key_ports			tp;
	struct flow_dissector_key_ip			ip;
	struct flow_dissector_key_vlan			vlan;
	struct flow_dissector_key_eth_addrs		eth_addrs;
} __aligned(BITS_PER_LONG / 8); /* Ensure that we can do comparisons as longs. */

struct nft_flow_match {
	struct flow_dissector	dissector;
	struct nft_flow_key	key;
	struct nft_flow_key	mask;
};

struct nft_flow_rule {
	__be16			proto;
	struct nft_flow_match	match;
	struct flow_rule	*rule;
};

#define NFT_OFFLOAD_F_ACTION	(1 << 0)

struct nft_rule;
struct nft_flow_rule *nft_flow_rule_create(const struct nft_rule *rule);
void nft_flow_rule_destroy(struct nft_flow_rule *flow);
int nft_flow_rule_offload_commit(struct net *net);
void nft_indr_block_get_and_ing_cmd(struct net_device *dev,
				    flow_indr_block_bind_cb_t *cb,
				    void *cb_priv,
				    enum flow_block_command command);

#define NFT_OFFLOAD_MATCH(__key, __base, __field, __len, __reg)		\
	(__reg)->base_offset	=					\
		offsetof(struct nft_flow_key, __base);			\
	(__reg)->offset		=					\
		offsetof(struct nft_flow_key, __base.__field);		\
	(__reg)->len		= __len;				\
	(__reg)->key		= __key;				\
	memset(&(__reg)->mask, 0xff, (__reg)->len);

#endif
