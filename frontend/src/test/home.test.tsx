import { describe, it, expect, vi } from 'vitest';
import { render, screen, fireEvent } from '@testing-library/react';
import { HomePage } from '../pages/HomePage';

describe('HomePage Component', () => {
  it('renders hero headline and supporting explanation', () => {
    const onStartInvestigation = vi.fn();
    const onOpenVerify = vi.fn();

    render(
      <HomePage
        onStartInvestigation={onStartInvestigation}
        onOpenVerify={onOpenVerify}
      />
    );

    expect(
      screen.getByText(/Automatically discover the inputs that break greedy algorithms./i)
    ).toBeInTheDocument();
    expect(
      screen.getByText(/Greedy Breaker systematically searches bounded input spaces/i)
    ).toBeInTheDocument();

    const startBtn = screen.getByRole('button', { name: /Start an Investigation/i });
    expect(startBtn).toBeInTheDocument();
    fireEvent.click(startBtn);
    expect(onStartInvestigation).toHaveBeenCalledTimes(1);
  });

  it('renders real counterexample section with Coin Change demonstration', () => {
    const onStartInvestigation = vi.fn();
    const onOpenVerify = vi.fn();

    render(
      <HomePage
        onStartInvestigation={onStartInvestigation}
        onOpenVerify={onOpenVerify}
      />
    );

    expect(screen.getByText(/See a greedy algorithm break./i)).toBeInTheDocument();
    expect(screen.getByText(/{1, 3, 4}/i)).toBeInTheDocument();
    expect(screen.getByText(/4 \+ 1 \+ 1/i)).toBeInTheDocument();
    expect(screen.getByText(/= 3 coins/i)).toBeInTheDocument();
    expect(screen.getByText(/3 \+ 3/i)).toBeInTheDocument();
    expect(screen.getByText(/= 2 coins/i)).toBeInTheDocument();

    const verifyBtn = screen.getByRole('button', { name: /Verify this instance live/i });
    expect(verifyBtn).toBeInTheDocument();
    fireEvent.click(verifyBtn);
    expect(onOpenVerify).toHaveBeenCalledTimes(1);
  });

  it('renders the 4 supported algorithm families and how it works steps', () => {
    render(
      <HomePage
        onStartInvestigation={vi.fn()}
        onOpenVerify={vi.fn()}
      />
    );

    expect(
      screen.getByText(/Four algorithm families. One investigation framework./i)
    ).toBeInTheDocument();
    expect(screen.getByText(/Activity Selection/i)).toBeInTheDocument();
    expect(screen.getByText(/0\/1 Knapsack/i)).toBeInTheDocument();
    expect(screen.getByText(/Fractional Knapsack/i)).toBeInTheDocument();

    expect(screen.getByText(/01/i)).toBeInTheDocument();
    expect(screen.getByText(/Generate Candidates/i)).toBeInTheDocument();
    expect(screen.getByText(/02/i)).toBeInTheDocument();
    expect(screen.getByText(/Run Greedy Strategy/i)).toBeInTheDocument();
    expect(screen.getByText(/03/i)).toBeInTheDocument();
    expect(screen.getByText(/Compare Against Exact Solver/i)).toBeInTheDocument();
    expect(screen.getByText(/04/i)).toBeInTheDocument();
    expect(screen.getByText(/Analyze the Failure/i)).toBeInTheDocument();

    expect(
      screen.getByText(/Greedy Breaker does not assume the greedy answer is correct./i)
    ).toBeInTheDocument();
  });
});
