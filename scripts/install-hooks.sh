#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Create hooks directory if it doesn't exist
mkdir -p "$PROJECT_ROOT/.git/hooks"

# Install pre-commit hook
echo "Installing pre-commit hook..."
cp "$SCRIPT_DIR/hooks/pre-commit" "$PROJECT_ROOT/.git/hooks/"
chmod +x "$PROJECT_ROOT/.git/hooks/pre-commit"

# Verify installation
if [ -x "$PROJECT_ROOT/.git/hooks/pre-commit" ]; then
    echo "Git hooks installed successfully!"
else
    echo "Failed to install git hooks!"
    exit 1
fi