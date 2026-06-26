#pragma once

#include <cstddef>
#include <string>

namespace network_server::model {

/**
 * @brief Descriptor of a packet message body.
 *
 * It stores the message length and owns the message area through std::string.
 */
class MessageDescriptor {
public:
    /**
     * @brief Creates an empty message descriptor.
     */
    MessageDescriptor() = default;

    /**
     * @brief Creates a descriptor for the provided message content.
     */
    explicit MessageDescriptor(std::string content);

    /**
     * @brief Returns the message length in bytes.
     */
    std::size_t length() const noexcept;

    /**
     * @brief Returns the message content.
     */
    const std::string& content() const noexcept;

private:
    std::string content_;
};

}  // namespace network_server::model
