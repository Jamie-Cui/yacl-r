# Security Policy

## Supported Versions

The following versions of YACL-r are currently being supported with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 0.4.x   | :white_check_mark: |
| < 0.4   | :x:                |

## Reporting a Vulnerability

**Please do not report security vulnerabilities through public GitHub issues.**

### How to Report

Instead, please report security vulnerabilities by emailing:

- **Email**: security@example.com (replace with actual security contact)

Include the following information:

1. **Description**: A clear description of the vulnerability
2. **Impact**: What an attacker could achieve by exploiting this vulnerability
3. **Reproduction**: Steps to reproduce the issue
4. **Proof of Concept**: If available, a minimal example demonstrating the issue
5. **Affected Versions**: Which versions are affected
6. **Fix Suggestion**: If you have suggestions for fixing the issue

### What to Expect

- **Acknowledgment**: We will acknowledge receipt of your report within 48 hours
- **Initial Assessment**: We will provide an initial assessment within 5 business days
- **Updates**: We will keep you informed of our progress
- **Disclosure**: We will coordinate disclosure with you

### Disclosure Policy

- We follow responsible disclosure practices
- We will not disclose details until a fix is available
- We will credit you in the security advisory (unless you prefer to remain anonymous)

## Security Considerations

### Important Notes

YACL-r is a cryptographic library designed for research purposes. Please be aware:

1. **Research Use**: This library is primarily intended for research and educational purposes. Some implementations may not be suitable for production use.

2. **Known Issues**: Check the [Issues](https://github.com/Jamie-Cui/yacl-r/issues) page for any known security concerns.

3. **Algorithm Warnings**: Some algorithms have known limitations:
   - KOS OTe has potential security flaws (see CHANGELOG)
   - Experimental code in `yacl/crypto/experimental/` should be used with caution

### Best Practices

When using YACL-r:

- Use the latest supported version
- Review algorithm documentation before use
- Do not use experimental code in production systems
- Keep dependencies up to date
- Follow cryptographic best practices

## Security Features

YACL-r implements several security features:

- **Secure Memory Handling**: Sensitive data is cleared when no longer needed
- **Constant-Time Operations**: Critical operations use constant-time implementations where applicable
- **Memory Safety**: C++17 features are used to minimize memory safety issues
- **Compiler Hardening**: Security flags are enabled by default (stack protection, FORTIFY_SOURCE)

## Security Updates

Security updates will be released as patch versions within supported minor versions.

Subscribe to [GitHub Releases](https://github.com/Jamie-Cui/yacl-r/releases) to be notified of security updates.