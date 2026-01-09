#!/usr/bin/env python3

import argparse


def main():
    parser = argparse.ArgumentParser(
        description="ExampleAgent: sample micro agent"
    )
    parser.add_argument(
        '--message', '-m',
        help='Custom message to print',
        default='Hello from ExampleAgent!'
    )
    args = parser.parse_args()
    print(args.message)


if __name__ == '__main__':
    main()
