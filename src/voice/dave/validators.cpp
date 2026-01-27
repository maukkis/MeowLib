#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h"
#include <cstdint>
#include <mlspp/tls/tls_syntax.h>
#include <mlspp/mls/messages.h>


bool Dave::isValidProposal(const mls::ValidatedContent& a){
  if(a.authenticated_content().content.content_type() != mls::ContentType::proposal){
    Log::error("not a proposal");
    return false;
  }
  auto proposal = std::get<mls::Proposal>(a.authenticated_content().content.content);
  // only mls add and remove proposals
  if(proposal.proposal_type() != mls::ProposalType::add && proposal.proposal_type() != mls::ProposalType::remove){
    Log::error("not valid proposal type");
    return false;
  }
  if(a.authenticated_content().content.sender.sender_type() != mls::SenderType::external){
    Log::error("proposal from a sender that is not the voice gateway");
    return false;
  }
  
  if(proposal.proposal_type() == mls::ProposalType::add){
    auto add = std::get<mls::Add>(proposal.content);
    uint64_t snowflake = snowflakeFromCredential(add.key_package.leaf_node.credential.get<mls::BasicCredential>().identity);
    if(!users.contains(snowflake)){
      Log::error("proposing to add a member to the group who shouldnt be in the group");
      return false;
    }
  }

  return true;
}

bool Dave::isValidCommit(const mls::ValidatedContent& a){
  if(a.authenticated_content().content.content_type() != mls::ContentType::commit){
    Log::error("not a commit");
    return false;
  }
  auto commit = std::get<mls::Commit>(a.authenticated_content().content.content);
  for(const auto& pro : commit.proposals){
    if(mls::tls::variant<mls::ProposalOrRefType>::type(pro.content) != mls::ProposalOrRefType::reference){
      Log::error("trying to introduce an unknown proposal to the commit");
      return false;
    }
  }
  return true;
}


bool Dave::isValidWelcomeState(){
  if(currentState->extensions().extensions.size() != 1){
    Log::error("state has more than one extension");
    return false;
  }
  auto extension = currentState->extensions().find<mls::ExternalSendersExtension>();
  if(extension->senders.size() != 1){
    Log::error("more than one external sender");
    return false;
  }
  if(extension->senders.at(0) != *externalSender){
    Log::error("state has a different external sender than what the voice gateway sent");
    return false;
  }
  if(currentState->roster().size() != users.size()){
    Log::error("not the same amount of users in the group");
    return false;
  }
  
  for(const auto& leaf : currentState->roster()){
    uint64_t snowflake = snowflakeFromCredential(leaf.credential.get<mls::BasicCredential>().identity);
    if(!users.contains(snowflake)){
      Log::error("snowflake in group thats not supposed to be there");
      return false;
    }
  }

  return true;
}
