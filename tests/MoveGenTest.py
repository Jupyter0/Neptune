import subprocess
import pathlib
import time

PATH = pathlib.Path(__file__).parent.parent.resolve()
ENGINE = PATH.joinpath("/bin/Neptune")
STOCKFISH = PATH.joinpath("/bin/stockfish-ubuntu-x86-64-avx2")

class FenTests:
    test_fens = [
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pppppppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",
        "rnbqkbnr/pppp1ppp/8/4p3/3PP3/8/PPP2PPP/RNBQKBNR b KQkq d3 0 2",
        "rnbqkbnr/pppp1ppp/8/8/3pP3/5N2/PPP2PPP/RNBQKB1R b KQkq - 1 3",
        "rnbqk1nr/pppp1ppp/8/8/3pP3/5N2/PPP2PPP/RNBQKB1R w KQkq - 2 4",
        "rnbqk2r/pppp1ppp/5n2/4P3/8/5N2/PPP2PPP/RNBQKB1R w KQkq - 5 5",
        "rnbqk2r/pppp1ppp/5n2/4P3/8/5N2/PPP2PPP/RNBQK2R b KQkq - 5 5",
        "r3kbnr/pppb1ppp/2n5/3qp3/3P4/2N2N2/PPP1QPPP/R1B1KB1R w KQkq - 8 8",
        "r3k2r/pppb1ppp/2n5/3qp3/3P4/2N2N2/PPP1QPPP/R1B1KB1R b KQkq - 8 8",
        "rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPP2PPP/RNBQKBNR w KQkq d6 0 3",
        "8/P7/K7/8/8/8/7p/7k w - - 0 1",
        "8/1P6/K7/8/8/8/7p/7k w - - 0 1",
        "8/8/8/8/8/k7/p7/K7 b - - 0 1",
        "8/8/8/4K3/8/8/P7/k7 b - - 0 1",
        "8/P7/8/8/3K4/8/8/7k w - - 0 1",
        "8/1P6/1K6/8/8/8/8/7k w - - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPP1KPPP/RNBQ1BNR b kq - 0 1",
        "rnbqkbnr/pppppppp/8/4P3/8/8/PPP1KPPP/RNBQ1BNR b kq - 0 1",
        "r1bqkbnr/pppppppp/2n5/8/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3",
        "rnbqkbnr/pppp1ppp/8/4p3/3PP3/8/PPP2PPP/RNBQKBNR b KQkq d3 0 2",
        "rnbqkb1r/pppppppp/5n2/8/3P4/5N2/PPP2PPP/RNBQKB1R b KQkq - 3 3",
        "r1bqkbnr/pp1ppppp/2n5/2p5/3PP3/8/PPP2PPP/RNBQKBNR w KQkq c6 0 3",
        "rnb1kbnr/ppppqppp/8/4p3/3PP3/5N2/PPP2PPP/RNBQKB1R w KQkq - 2 3",
        "rnbqkbnr/pppp1ppp/8/4p3/3PP3/8/PPP2PPP/RNBQKBNR b KQkq d3 0 2",
        "rnbqkbnr/pppp1ppp/8/8/3pP3/5N2/PPP2PPP/RNBQKB1R b KQkq - 1 3",
        "r1bqkbnr/pppppppp/2n5/8/8/2N2B2/PPPPPPPP/R1BQK1NR w KQkq - 4 4",
        "2kr3r/ppp2ppp/2np1n2/8/3PP3/2N1B3/PPP2PPP/R3K2R w KQ - 4 10",
        "r3k2r/ppp2ppp/2n5/3qp3/3P4/2N2N2/PPP1QPPP/R1B1KB1R w KQkq - 0 9",
        "rnbqkbnr/pppp1ppp/4p3/8/2PP4/5N2/PP2PPPP/RNBQKB1R b KQ - 0 3",
        "rnb1kbnr/ppppqppp/8/4p3/3PP3/5N2/PPP2PPP/RNBQKB1R w KQkq - 2 3",
        "6k1/5ppp/8/8/8/8/5PPP/6KR w - - 0 1",
        "7k/5K2/6Q1/8/8/8/8/8 b - - 0 1",
        "8/2k5/8/8/8/8/8/2K5 w - - 0 1",
        "8/2k5/8/8/8/8/8/2K1N3 w - - 0 1",
        "8/1P6/8/8/8/8/8/k6K w - - 0 1",
        "rnbqkbnr/pppp1ppp/8/4p3/3PP3/2N5/PPP2PPP/R1BQKBNR b KQkq - 2 3",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pppppppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",
        "r1bqkbnr/pppppppp/2n5/8/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3",
        "rnbqkbnr/pppp1ppp/8/4p3/3PP3/8/PPP2PPP/RNBQKBNR b KQkq d3 0 2",
        "rnbqkb1r/pppppppp/5n2/8/3P4/5N2/PPP2PPP/RNBQKB1R b KQkq - 3 3",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1",
        "8/8/1k6/8/8/8/1P6/1K6 w - - 0 1",
        "8/8/8/5K2/8/8/6p1/6k1 b - - 0 1",
        "8/8/3k4/8/3pP3/8/8/4K3 b - e3 0 1",
        "8/8/8/4P3/3pk3/8/8/4K3 w - - 0 2",
        "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
    ]

    moveLists = [
        "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 e1g1 f8e7 f1e1 b7b5 a4b3 e8g8 c2c3 d7d5 e4d5 f6d5 f3e5 c6e5 e1e5 c7c6 d2d4 e7d6 e5e1 d8h4 g2g3 h4h3 c1e3 c8g4 d1d3 a8e8 b1d2 e8e6 a2a4 h3h5"
    ]

    legalMoves = [
        ['a2a3', 'b2b3', 'c2c3', 'd2d3', 'e2e3', 'f2f3', 'g2g3', 'h2h3', 'a2a4', 'b2b4', 'c2c4', 'd2d4', 'e2e4', 'f2f4', 'g2g4', 'h2h4', 'b1a3', 'b1c3', 'g1f3', 'g1h3'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'e7e5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6'],
        ['a2a3', 'b2b3', 'c2c3', 'd2d3', 'f2f3', 'g2g3', 'h2h3', 'a2a4', 'b2b4', 'c2c4', 'd2d4', 'f2f4', 'g2g4', 'h2h4', 'b1a3', 'b1c3', 'g1e2', 'g1f3', 'g1h3', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'd1e2', 'd1f3', 'd1g4', 'd1h5', 'e1e2'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'e5d4', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['d4d3', 'a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['a2a3', 'b2b3', 'c2c3', 'g2g3', 'h2h3', 'e4e5', 'a2a4', 'b2b4', 'c2c4', 'g2g4', 'h2h4', 'b1d2', 'b1a3', 'b1c3', 'f3g1', 'f3d2', 'f3d4', 'f3h4', 'f3e5', 'f3g5', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'h1g1', 'd1d2', 'd1e2', 'd1d3', 'd1d4', 'e1d2', 'e1e2'],
        ['a2a3', 'b2b3', 'c2c3', 'g2g3', 'h2h3', 'e5e6', 'a2a4', 'b2b4', 'c2c4', 'g2g4', 'h2h4', 'e5f6', 'b1d2', 'b1a3', 'b1c3', 'f3g1', 'f3d2', 'f3d4', 'f3h4', 'f3g5', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'h1g1', 'd1d2', 'd1e2', 'd1d3', 'd1d4', 'd1d5', 'd1d6', 'd1d7', 'e1d2', 'e1e2'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'g7g5', 'h7h5', 'f6e4', 'f6g4', 'f6d5', 'f6h5', 'f6g8', 'b8a6', 'b8c6', 'h8f8', 'h8g8', 'd8e7', 'e8e7', 'e8f8', 'e8g8'],
        ['a2a3', 'b2b3', 'g2g3', 'h2h3', 'a2a4', 'b2b4', 'g2g4', 'h2h4', 'd4e5', 'c3b1', 'c3d1', 'c3a4', 'c3e4', 'c3b5', 'c3d5', 'f3g1', 'f3d2', 'f3h4', 'f3e5', 'f3g5', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'a1b1', 'h1g1', 'e2d1', 'e2d2', 'e2d3', 'e2e3', 'e2c4', 'e2e4', 'e2b5', 'e2e5', 'e2a6', 'e1d1', 'e1d2'],
        ['e5e4', 'a7a6', 'b7b6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'f7f5', 'g7g5', 'h7h5', 'e8c8', 'c6b4', 'c6d4', 'c6a5', 'c6e7', 'c6b8', 'c6d8', 'd7h3', 'd7g4', 'd7f5', 'd7e6', 'd7c8', 'a8b8', 'a8c8', 'a8d8', 'h8f8', 'h8g8', 'd5a2', 'd5b3', 'd5f3', 'd5c4', 'd5d4', 'd5e4', 'd5a5', 'd5b5', 'd5c5', 'd5d6', 'd5e6', 'e8e7', 'e8d8', 'e8f8', 'e8g8'],
        ['a2a3', 'b2b3', 'c2c3', 'f2f3', 'g2g3', 'h2h3', 'e5e6', 'a2a4', 'b2b4', 'c2c4', 'f2f4', 'g2g4', 'h2h4', 'e5d6', 'b1d2', 'b1a3', 'b1c3', 'g1e2', 'g1f3', 'g1h3', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'd1d2', 'd1e2', 'd1d3', 'd1f3', 'd1d4', 'd1g4', 'd1d5', 'd1h5', 'e1d2', 'e1e2'],
        ['a7a8q', 'a7a8r', 'a7a8b', 'a7a8n', 'a6a5', 'a6b5', 'a6b6', 'a6b7'],
        ['b7b8q', 'b7b8r', 'b7b8b', 'b7b8n', 'a6a5', 'a6b5', 'a6b6', 'a6a7'],
        ['a3b4', 'a3b3', 'a3a4'],
        ['a1b1', 'a1a2', 'a1b2'],
        ['a7a8q', 'a7a8r', 'a7a8b', 'a7a8n', 'd4c3', 'd4d3', 'd4e3', 'd4c4', 'd4e4', 'd4c5', 'd4d5', 'd4e5'],
        ['b7b8q', 'b7b8r', 'b7b8b', 'b7b8n', 'b6a5', 'b6b5', 'b6c5', 'b6a6', 'b6c6', 'b6a7', 'b6c7'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'e7e5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6'],
        ['a2a3', 'b2b3', 'c2c3', 'd2d3', 'g2g3', 'h2h3', 'e4e5', 'a2a4', 'b2b4', 'c2c4', 'd2d4', 'g2g4', 'h2h4', 'b1a3', 'b1c3', 'f3g1', 'f3d4', 'f3h4', 'f3e5', 'f3g5', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'h1g1', 'd1e2', 'e1e2'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'e5d4', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'e7e5', 'g7g5', 'h7h5', 'f6e4', 'f6g4', 'f6d5', 'f6h5', 'f6g8', 'b8a6', 'b8c6', 'h8g8'],
        ['a2a3', 'b2b3', 'c2c3', 'f2f3', 'g2g3', 'h2h3', 'd4d5', 'e4e5', 'a2a4', 'b2b4', 'c2c4', 'f2f4', 'g2g4', 'h2h4', 'd4c5', 'b1d2', 'b1a3', 'b1c3', 'g1e2', 'g1f3', 'g1h3', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'd1d2', 'd1e2', 'd1d3', 'd1f3', 'd1g4', 'd1h5', 'e1d2', 'e1e2'],
        ['a2a3', 'b2b3', 'c2c3', 'g2g3', 'h2h3', 'd4d5', 'a2a4', 'b2b4', 'c2c4', 'g2g4', 'h2h4', 'd4e5', 'b1d2', 'b1a3', 'b1c3', 'f3g1', 'f3d2', 'f3h4', 'f3e5', 'f3g5', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'h1g1', 'd1d2', 'd1e2', 'd1d3', 'e1d2', 'e1e2'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'e5d4', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['d4d3', 'a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['a2a3', 'b2b3', 'd2d3', 'e2e3', 'g2g3', 'h2h3', 'a2a4', 'b2b4', 'd2d4', 'e2e4', 'g2g4', 'h2h4', 'g1h3', 'c3b1', 'c3a4', 'c3e4', 'c3b5', 'c3d5', 'f3e4', 'f3g4', 'f3d5', 'f3h5', 'f3c6', 'a1b1', 'e1f1'],
        ['a2a3', 'b2b3', 'f2f3', 'g2g3', 'h2h3', 'd4d5', 'e4e5', 'a2a4', 'b2b4', 'f2f4', 'g2g4', 'h2h4', 'c3b1', 'c3d1', 'c3e2', 'c3a4', 'c3b5', 'c3d5', 'e3c1', 'e3d2', 'e3f4', 'e3g5', 'e3h6', 'a1b1', 'a1c1', 'a1d1', 'h1f1', 'h1g1', 'e1d1', 'e1f1', 'e1d2', 'e1e2', 'e1g1', 'e1c1'],
        ['a2a3', 'b2b3', 'g2g3', 'h2h3', 'a2a4', 'b2b4', 'g2g4', 'h2h4', 'd4e5', 'c3b1', 'c3d1', 'c3a4', 'c3e4', 'c3b5', 'c3d5', 'f3g1', 'f3d2', 'f3h4', 'f3e5', 'f3g5', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'a1b1', 'h1g1', 'e2d1', 'e2d2', 'e2d3', 'e2e3', 'e2c4', 'e2e4', 'e2b5', 'e2e5', 'e2a6', 'e1d1', 'e1d2'],
        ['e6e5', 'a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['a2a3', 'b2b3', 'c2c3', 'g2g3', 'h2h3', 'd4d5', 'a2a4', 'b2b4', 'c2c4', 'g2g4', 'h2h4', 'd4e5', 'b1d2', 'b1a3', 'b1c3', 'f3g1', 'f3d2', 'f3h4', 'f3e5', 'f3g5', 'c1d2', 'c1e3', 'c1f4', 'c1g5', 'c1h6', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'h1g1', 'd1d2', 'd1e2', 'd1d3', 'e1d2', 'e1e2'],
        ['f2f3', 'g2g3', 'h2h3', 'f2f4', 'g2g4', 'h2h4', 'g1f1'],
        [],
        ['c1b1', 'c1d1', 'c1b2', 'c1c2', 'c1d2'],
        ['e1c2', 'e1g2', 'e1d3', 'e1f3', 'c1b1', 'c1d1', 'c1b2', 'c1c2', 'c1d2'],
        ['b7b8q', 'b7b8r', 'b7b8b', 'b7b8n', 'h1g1', 'h1g2', 'h1h2'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'e5d4', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'e7e5', 'f7f5', 'g7g5', 'h7h5', 'b8a6', 'b8c6', 'g8f6', 'g8h6'],
        ['a2a3', 'b2b3', 'c2c3', 'd2d3', 'f2f3', 'g2g3', 'h2h3', 'a2a4', 'b2b4', 'c2c4', 'd2d4', 'f2f4', 'g2g4', 'h2h4', 'b1a3', 'b1c3', 'g1e2', 'g1f3', 'g1h3', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'd1e2', 'd1f3', 'd1g4', 'd1h5', 'e1e2'],
        ['a2a3', 'b2b3', 'c2c3', 'd2d3', 'g2g3', 'h2h3', 'e4e5', 'a2a4', 'b2b4', 'c2c4', 'd2d4', 'g2g4', 'h2h4', 'b1a3', 'b1c3', 'f3g1', 'f3d4', 'f3h4', 'f3e5', 'f3g5', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'h1g1', 'd1e2', 'e1e2'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'f7f6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'f7f5', 'g7g5', 'h7h5', 'e5d4', 'b8a6', 'b8c6', 'g8f6', 'g8h6', 'g8e7', 'f8a3', 'f8b4', 'f8c5', 'f8d6', 'f8e7', 'd8h4', 'd8g5', 'd8f6', 'd8e7', 'e8e7'],
        ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'g7g6', 'h7h6', 'a7a5', 'b7b5', 'c7c5', 'd7d5', 'e7e5', 'g7g5', 'h7h5', 'f6e4', 'f6g4', 'f6d5', 'f6h5', 'f6g8', 'b8a6', 'b8c6', 'h8g8'],
        ['a2a3', 'b2b3', 'c2c3', 'd2d3', 'f2f3', 'g2g3', 'h2h3', 'e4e5', 'a2a4', 'b2b4', 'c2c4', 'd2d4', 'f2f4', 'g2g4', 'h2h4', 'b1a3', 'b1c3', 'g1e2', 'g1f3', 'g1h3', 'f1e2', 'f1d3', 'f1c4', 'f1b5', 'f1a6', 'd1e2', 'd1f3', 'd1g4', 'd1h5', 'e1e2'],
        ['b2b3', 'b2b4', 'b1a1', 'b1c1', 'b1a2', 'b1c2'],
        ['g1f1', 'g1h1', 'g1f2', 'g1h2'],
        ['d4d3', 'd4e3', 'd6c5', 'd6e7', 'd6e5', 'd6c6', 'd6e6', 'd6c7', 'd6d7'],
        ['e5e6', 'e1d1', 'e1f1', 'e1d2', 'e1e2', 'e1f2'],
        ['a1b1', 'a1c1', 'a1d1', 'a1a2', 'a1a3', 'a1a4', 'a1a5', 'a1a6', 'a1a7', 'a1a8', 'h1f1', 'h1g1', 'h1h2', 'h1h3', 'h1h4', 'h1h5', 'h1h6', 'h1h7', 'h1h8', 'e1d1', 'e1f1', 'e1d2', 'e1e2', 'e1f2', 'e1g1', 'e1c1'],
    ]

if __name__ == "__main__":
    for i in range(FenTests.test_fens.__len__()):
        proc = subprocess.Popen(f".{ENGINE}", stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=None, text=True, bufsize=1)
        proc.stdin.write("ucinewgame\n")
        proc.stdin.write(f"position fen {FenTests.test_fens[i]}\n")
        proc.stdin.write("go perft 1\n")
        proc.stdin.flush()

        start = time.time()
        moves = []
        while True:
            line = proc.stdout.readline()
            if "Found: " in line:
                break
            if line == '' and proc.poll() is not None:
                print("<<< Engine exited unexpectedly.")
                quit(-1)
            elif line.strip():
                if "Stock" in line: continue
                if "info" in line: continue
                if "Legal" in line: continue
                move = line[0:5].strip(":")
                moves.append(move.strip())
            if time.time() - start > 5:
                print("<<< Timeout waiting for Engine.")
                break
        if set(moves) != set(FenTests.legalMoves[i]):
            print("\nMismatch!")
            print(FenTests.test_fens[i])
            print(f"Engine Output: {moves}")
        proc.stdin.close()
    print("Done")