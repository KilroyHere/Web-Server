// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <utility>
#include <boost/log/trivial.hpp>

#include "config_parser.h"

bool NginxConfig::query_config(std::vector<std::string> query, std::string &value)
{
  if (query.size() == 0)
    return false;
  std::string target = *query.begin();

  for (auto statement : statements_)
  {
    for (int i = 0; i < statement->tokens_.size(); i++)
    {
      std::string token = statement->tokens_[i];
      if (token == target)
      {
        if (query.size() == 1)
        {
          if (i + 1 < statement->tokens_.size())
          {
            value = statement->tokens_[i + 1];
            return true;
          }
          else
          {
            return false;
          }
        }
        if (statement->child_block_ != nullptr)
        {
          bool found;
          query.erase(query.begin());
          return (statement->child_block_->query_config(query, value));
        }
      }
    }
  }
  return false;
}

bool NginxConfig::query_config(std::vector<std::string> query, std::vector<std::string> &values)
{
  if (query.empty())
    return false;

  bool found = false;

  // Stack stores the NginxConfig blocks to visit and the corresponding statement index
  std::stack<std::pair<NginxConfig *, size_t>> to_visit;

  // Start with the current config and statement index 0
  to_visit.push({this, 0});

  while (!to_visit.empty())
  {
    // Get the current config and index from the top of the stack
    NginxConfig *current_config = to_visit.top().first;
    size_t &index = to_visit.top().second;

    // If the index is out of range, remove the current config from the stack and continue
    if (index >= current_config->statements_.size())
    {
      to_visit.pop();
      continue;
    }

    // Get the current statement
    const auto &statement = current_config->statements_[index];
    size_t i = 0;

    // Iterate through the tokens of the statement
    while (i < statement->tokens_.size())
    {
      // Check if the token matches the first element of the query
      if (statement->tokens_[i] == query.front())
      {
        // If there's only one element in the query, add the subsequent tokens to the result
        if (query.size() == 1)
        {
          for (size_t j = i + 1; j < statement->tokens_.size(); ++j)
          {
            values.push_back(statement->tokens_[j]);
            found = true;
          }
        }
        // If the statement has a child block, push it onto the stack and remove the first element of the query
        else if (statement->child_block_)
        {
          query.erase(query.begin());
          to_visit.push({statement->child_block_.get(), 0});
        }
      }
      ++i;
    }

    // If the top of the stack still has the same config, increment the index
    if (!to_visit.empty() && to_visit.top().first == current_config)
    {
      ++to_visit.top().second;

      // If the query size is greater than 1 and we are going up a level, restore the first element of the query
      if (query.size() > 1 && index == 0)
      {
        query.insert(query.begin(), query.front());
      }
    }
  }

  return found;
}

std::string NginxConfig::ToString(int depth)
{
  std::string serialized_config;
  for (const auto &statement : statements_)
  {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

std::string NginxConfigStatement::ToString(int depth)
{
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i)
  {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i)
  {
    if (i != 0)
    {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr)
  {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i)
    {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  }
  else
  {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

bool NginxConfig::relative_path_query(std::vector<std::string> query, std::string &value, int index)
{
  std::stack<std::pair<NginxConfig *, int>> mstack;
  for (auto statement : statements_)
  {
    if (query.size() - 1 > index && statement->child_block_.get() != nullptr)
    {
      if (query[index] == statement->tokens_[0])
      {
        std::pair<NginxConfig *, int> block(statement->child_block_.get(), index + 1);
        mstack.push(block);
      }
      else
      {
        std::pair<NginxConfig *, int> block(statement->child_block_.get(), index);
        mstack.push(block);
      }
    }
    else
    {
      if (query.size() - 1 == index && statement->tokens_.size() > 1 && statement->tokens_[0] == query[index])
      {
        value = statement->tokens_[1].c_str();
        return true;
      }
      else
      {
        if (statement->child_block_.get() != nullptr)
        {
          std::pair<NginxConfig *, int> block(statement->child_block_.get(), index);
          mstack.push(block);
        }
      }
    }
  }
  while (!mstack.empty())
  {
    std::pair<NginxConfig *, int> curr = mstack.top();
    mstack.pop();
    if (curr.second <= query.size() && curr.first->relative_path_query(query, value, curr.second) == true)
    {
      return true;
    }
  }

  return false;
}

bool NginxConfig::config_port_num(std::vector<std::string> query, std::string &value)
{
  std::string result;
  if (!relative_path_query(query, result, 0))
  {
    return false;
  }
  int port = atoi(result.c_str());
  // valid port number
  if (0 < port && 65536 > port)
  {
    value = result.c_str();
    return true;
  }
  // invalid port number
  else
  {
    return false;
  }
}

const char *NginxConfigParser::TokenTypeAsString(TokenType type)
{
  switch (type)
  {
  case TOKEN_TYPE_START:
    return "TOKEN_TYPE_START";
  case TOKEN_TYPE_NORMAL:
    return "TOKEN_TYPE_NORMAL";
  case TOKEN_TYPE_START_BLOCK:
    return "TOKEN_TYPE_START_BLOCK";
  case TOKEN_TYPE_END_BLOCK:
    return "TOKEN_TYPE_END_BLOCK";
  case TOKEN_TYPE_COMMENT:
    return "TOKEN_TYPE_COMMENT";
  case TOKEN_TYPE_STATEMENT_END:
    return "TOKEN_TYPE_STATEMENT_END";
  case TOKEN_TYPE_EOF:
    return "TOKEN_TYPE_EOF";
  case TOKEN_TYPE_ERROR:
    return "TOKEN_TYPE_ERROR";
  default:
    return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream *input,
                                                           std::string *value)
{
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good())
  {
    const char c = input->get();
    if (!input->good())
    {
      break;
    }
    switch (state)
    {
    case TOKEN_STATE_INITIAL_WHITESPACE:
      switch (c)
      {
      case '{':
        *value = c;
        return TOKEN_TYPE_START_BLOCK;
      case '}':
        *value = c;
        return TOKEN_TYPE_END_BLOCK;
      case '#':
        *value = c;
        state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
        continue;
      case '"':
        *value = c;
        state = TOKEN_STATE_DOUBLE_QUOTE;
        continue;
      case '\'':
        *value = c;
        state = TOKEN_STATE_SINGLE_QUOTE;
        continue;
      case ';':
        *value = c;
        return TOKEN_TYPE_STATEMENT_END;
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        continue;
      default:
        *value += c;
        state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
        continue;
      }
    case TOKEN_STATE_SINGLE_QUOTE:
      // TODO: Maybe also define a QUOTED_STRING token type.
      // TODO: Allow for backslash-escaping within strings.
      *value += c;
      if (c == '\'')
      {
        // the end of a quoted token should be followed by whitespace.
        const char next_char = input->get();
        if (next_char == ' ' || next_char == '\t' || next_char == '\n' || next_char == '\t' ||
            next_char == ';' || next_char == '{' || next_char == '}')
        {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        else
        {
          return TOKEN_TYPE_ERROR;
        }
      }
      continue;
    case TOKEN_STATE_DOUBLE_QUOTE:
      *value += c;
      if (c == '"')
      {
        // the end of a quoted token should be followed by whitespace.
        const char next_char = input->get();
        if (next_char == ' ' || next_char == '\t' || next_char == '\n' || next_char == '\t' ||
            next_char == ';' || next_char == '{' || next_char == '}')
        {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        else
        {
          return TOKEN_TYPE_ERROR;
        }
      }
      continue;
    case TOKEN_STATE_TOKEN_TYPE_COMMENT:
      if (c == '\n' || c == '\r')
      {
        return TOKEN_TYPE_COMMENT;
      }
      *value += c;
      continue;
    case TOKEN_STATE_TOKEN_TYPE_NORMAL:
      if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
          c == ';' || c == '{' || c == '}')
      {
        input->unget();
        return TOKEN_TYPE_NORMAL;
      }
      *value += c;
      continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE)
  {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream *config_file, NginxConfig *config)
{
  std::stack<NginxConfig *> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  int nesting_count = 0;
  while (true)
  {
    std::string token;
    token_type = ParseToken(config_file, &token);
    // printf("%s: %s\n", TokenTypeAsString(token_type), token.c_str());

    if (token_type == TOKEN_TYPE_ERROR)
    {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT)
    {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START)
    {
      // Error.
      break;
    }
    else if (token_type == TOKEN_TYPE_NORMAL)
    {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL)
      {
        if (last_token_type != TOKEN_TYPE_NORMAL)
        {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      }
      else
      {
        // Error.
        break;
      }
    }
    else if (token_type == TOKEN_TYPE_STATEMENT_END)
    {
      if (last_token_type != TOKEN_TYPE_NORMAL)
      {
        // Error.
        break;
      }
    }
    else if (token_type == TOKEN_TYPE_START_BLOCK)
    {
      nesting_count += 1;
      if (last_token_type != TOKEN_TYPE_NORMAL)
      {
        // Error.
        break;
      }
      NginxConfig *const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    }
    else if (token_type == TOKEN_TYPE_END_BLOCK)
    {
      nesting_count -= 1;
      if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
           last_token_type != TOKEN_TYPE_START_BLOCK &&
           last_token_type != TOKEN_TYPE_END_BLOCK) ||
          nesting_count < 0)
      {
        // Error.
        break;
      }
      config_stack.pop();
    }
    else if (token_type == TOKEN_TYPE_EOF)
    {
      if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
           last_token_type != TOKEN_TYPE_END_BLOCK &&
           last_token_type != TOKEN_TYPE_START) ||
          nesting_count != 0)
      {
        // Error.
        break;
      }
      return true;
    }
    else
    {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf("Bad transition from %s to %s\n",
         TokenTypeAsString(last_token_type),
         TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char *file_name, NginxConfig *config)
{
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good())
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to open config file: " << file_name;
    return false;
  }
  BOOST_LOG_TRIVIAL(info) << "Start parsing config file.";
  const bool return_value =
      Parse(dynamic_cast<std::istream *>(&config_file), config);
  config_file.close();
  return return_value;
}
